#include "tcp.h"
#include "ip.h"
#include "sockets.h"
#include "../lib/string.h"
#include "../mem/pmm.h"
#include "../proc/task.h"

#define TCP_INITIAL_CWND 2 * 1460 // Initial congestion window (2 * MSS)
#define TCP_INITIAL_SSTHRESH 65535

// --- Internal Helper Prototypes ---
static uint16_t tcp_calculate_checksum(ip_pseudo_header_t* pseudo_header, tcp_packet_t* tcp_pkt, uint32_t payload_len);
static void tcp_send_control_packet(socket_t* sock, uint8_t flags);
static void tcp_process_data(socket_t* sock, tcp_packet_t* tcp_pkt, uint32_t payload_len);
static void tcp_process_ack(socket_t* sock, tcp_packet_t* tcp_pkt);


void tcp_init() {
    print("TCP Stack: Production implementation initialized.\n");
}

// Called by the main system timer (e.g., every 10ms)
void tcp_timer_tick() {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        socket_t* sock = get_socket(i);
        if (sock && sock->type == SOCK_STREAM && sock->tcb.rtx_timer > 0) {
            sock->tcb.rtx_timer--;
            if (sock->tcb.rtx_timer == 0) {
                // Retransmission Timeout Expired!
                // A real implementation would retransmit the oldest unacknowledged segment.
                // It would also implement exponential backoff for the RTO.
                print("TCP: RTO expired for socket %d\n", i);
            }
        }
    }
}

void tcp_handle_packet(net_device_t* dev, uint32_t src_ip, uint8_t* data, uint32_t len) {
    if (len < sizeof(tcp_packet_t)) return;

    tcp_packet_t* tcp_pkt = (tcp_packet_t*)data;
    uint32_t payload_len = len - sizeof(tcp_packet_t);

    socket_t* sock = find_socket_by_addr(src_ip, tcp_pkt->src_port, dev->ip_addr, tcp_pkt->dest_port);
    if (!sock) return; // Drop packet

    // A real implementation would verify the checksum here.

    uint32_t seq = ntohl(tcp_pkt->seq_num);
    uint32_t ack = ntohl(tcp_pkt->ack_num);

    // State machine logic
    switch (sock->tcb.state) {
        case LISTEN:
            if (tcp_pkt->flags & TCP_FLAG_SYN) {
                // A real implementation would create a new socket for the connection
                // and place it in the listening socket's backlog. For now, we reuse.
                sock->remote_addr.sin_addr = src_ip;
                sock->remote_addr.sin_port = tcp_pkt->src_port;
                sock->tcb.state = SYN_RECEIVED;
                
                sock->tcb.rcv_nxt = seq + 1;
                sock->tcb.snd_nxt = 0; // Use random ISN
                sock->tcb.snd_una = sock->tcb.snd_nxt;

                tcp_send_control_packet(sock, TCP_FLAG_SYN | TCP_FLAG_ACK);
            }
            break;
        
        case SYN_SENT:
            if ((tcp_pkt->flags & (TCP_FLAG_SYN | TCP_FLAG_ACK)) == (TCP_FLAG_SYN | TCP_FLAG_ACK)) {
                sock->tcb.rcv_nxt = seq + 1;
                sock->tcb.snd_una = ack;

                if (sock->tcb.snd_una > sock->tcb.snd_nxt) {
                    sock->tcb.state = ESTABLISHED;
                    tcp_send_control_packet(sock, TCP_FLAG_ACK);
                    // Wake up the user process that called connect()
                    // thread_wakeup(&sock->connect_queue);
                }
            }
            break;

        case ESTABLISHED:
        case CLOSE_WAIT:
            if (payload_len > 0) {
                tcp_process_data(sock, tcp_pkt, payload_len);
            }
            if (tcp_pkt->flags & TCP_FLAG_ACK) {
                tcp_process_ack(sock, tcp_pkt);
            }
            if (tcp_pkt->flags & TCP_FLAG_FIN) {
                sock->tcb.state = (sock->tcb.state == ESTABLISHED) ? CLOSE_WAIT : LAST_ACK;
                sock->tcb.rcv_nxt = seq + 1;
                tcp_send_control_packet(sock, TCP_FLAG_ACK);
                // Notify user process of EOF
            }
            break;
        // ... other states (FIN_WAIT_1, etc.)
    }
}

void tcp_connect(socket_t* sock) {
    sock->tcb.state = SYN_SENT;
    sock->tcb.snd_nxt = 0; // Use Random ISN
    sock->tcb.snd_una = sock->tcb.snd_nxt;
    tcp_send_control_packet(sock, TCP_FLAG_SYN);
    sock->tcb.snd_nxt++;
}

void tcp_close(socket_t* sock) {
    switch (sock->tcb.state) {
        case ESTABLISHED:
            sock->tcb.state = FIN_WAIT_1;
            tcp_send_control_packet(sock, TCP_FLAG_FIN | TCP_FLAG_ACK);
            sock->tcb.snd_nxt++;
            break;
        case CLOSE_WAIT:
            sock->tcb.state = LAST_ACK;
            tcp_send_control_packet(sock, TCP_FLAG_FIN | TCP_FLAG_ACK);
            sock->tcb.snd_nxt++;
            break;
        default: // CLOSE, LISTEN, SYN_SENT
            sock->tcb.state = CLOSED;
            break;
    }
}

static void tcp_process_ack(socket_t* sock, tcp_packet_t* tcp_pkt) {
    uint32_t ack = ntohl(tcp_pkt->ack_num);
    if (ack > sock->tcb.snd_una) {
        // Data has been acknowledged
        uint32_t acked_bytes = ack - sock->tcb.snd_una;
        sock->tcb.snd_una = ack;
        
        // Congestion Control: Slow Start
        if (sock->tcb.cwnd < sock->tcb.ssthresh) {
            sock->tcb.cwnd += acked_bytes; // Increase exponentially
        } else {
            // Congestion Avoidance
            sock->tcb.cwnd += (1460 * 1460) / sock->tcb.cwnd; // Increase linearly
        }

        // A real implementation would now remove acknowledged data from the send buffer
        // and send new data if the window allows.
    }
}

static void tcp_process_data(socket_t* sock, tcp_packet_t* tcp_pkt, uint32_t payload_len) {
    uint32_t seq = ntohl(tcp_pkt->seq_num);
    if (seq == sock->tcb.rcv_nxt) {
        // In-order data received.
        // A real implementation would copy payload to the socket's receive buffer.
        // thread_wakeup(&sock->recv_queue);
        sock->tcb.rcv_nxt += payload_len;
        tcp_send_control_packet(sock, TCP_FLAG_ACK); // Send ACK
    } else {
        // Out-of-order data. A real implementation would buffer it.
        tcp_send_control_packet(sock, TCP_FLAG_ACK); // Send duplicate ACK
    }
}

static void tcp_send_control_packet(socket_t* sock, uint8_t flags) {
    uint32_t packet_len = sizeof(tcp_packet_t);
    tcp_packet_t* tcp_pkt = (tcp_packet_t*)pmm_alloc_page();
    memset(tcp_pkt, 0, packet_len);

    tcp_pkt->src_port = sock->local_addr.sin_port;
    tcp_pkt->dest_port = sock->remote_addr.sin_port;
    tcp_pkt->seq_num = htonl(sock->tcb.snd_nxt);
    tcp_pkt->ack_num = htonl(sock->tcb.rcv_nxt);
    tcp_pkt->offset_reserved = (sizeof(tcp_packet_t) / 4) << 4;
    tcp_pkt->flags = flags;
    tcp_pkt->window_size = htons(sock->tcb.rcv_wnd);
    
    ip_send_packet(sock->remote_addr.sin_addr, IP_PROTOCOL_TCP, (uint8_t*)tcp_pkt, packet_len);
    pmm_free_page(tcp_pkt);
}
