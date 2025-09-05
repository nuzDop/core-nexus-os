/* kernel/src/net/tcp.c */

#include "tcp.h"
#include "ip.h"
#include "sockets.h"
#include "../lib/string.h"
#include "../mem/pmm.h"

// A simplified TCP implementation. A real one would need robust TCB management,
// retransmission timers, congestion control, and sliding window implementation.

// Helper to send a TCP packet
void tcp_send_packet(socket_t* sock, uint32_t seq, uint32_t ack, uint8_t flags, uint8_t* payload, uint32_t payload_len) {
    uint32_t packet_len = sizeof(tcp_packet_t) + payload_len;
    tcp_packet_t* tcp_pkt = (tcp_packet_t*)pmm_alloc_page();
    memset(tcp_pkt, 0, packet_len);

    tcp_pkt->src_port = sock->local_addr.sin_port;
    tcp_pkt->dest_port = sock->remote_addr.sin_port;
    tcp_pkt->seq_num = htonl(seq);
    tcp_pkt->ack_num = htonl(ack);
    tcp_pkt->offset_reserved = (sizeof(tcp_packet_t) / 4) << 4;
    tcp_pkt->flags = flags;
    tcp_pkt->window_size = htons(8192); // 8KB window

    // A real implementation would calculate a checksum over a pseudo-header
    tcp_pkt->checksum = 0;

    if (payload && payload_len > 0) {
        memcpy((uint8_t*)tcp_pkt + sizeof(tcp_packet_t), payload, payload_len);
    }
    
    ip_send_packet(sock->remote_addr.sin_addr, IP_PROTOCOL_TCP, (uint8_t*)tcp_pkt, packet_len);
    pmm_free_page(tcp_pkt);
}

// Main entry point for incoming TCP packets from the IP layer
void tcp_handle_packet(net_device_t* dev, uint32_t src_ip, uint8_t* data, uint32_t len) {
    if (len < sizeof(tcp_packet_t)) return;

    tcp_packet_t* tcp_pkt = (tcp_packet_t*)data;
    uint16_t src_port = tcp_pkt->src_port;
    uint16_t dest_port = tcp_pkt->dest_port;

    socket_t* sock = find_socket_by_addr(src_ip, src_port, dev->ip_addr, dest_port);
    if (!sock) {
        // No matching socket found
        return;
    }
    
    uint32_t seq = ntohl(tcp_pkt->seq_num);
    uint32_t ack = ntohl(tcp_pkt->ack_num);

    switch (sock->tcp_state) {
        case LISTEN:
            if (tcp_pkt->flags & TCP_FLAG_SYN) {
                // Received SYN on a listening socket, start handshake
                sock->remote_addr.sin_addr = src_ip;
                sock->remote_addr.sin_port = src_port;
                sock->tcp_state = SYN_RECEIVED;
                sock->ack_num = seq + 1;
                sock->seq_num = 0; // Initial sequence number
                
                // Send SYN-ACK
                tcp_send_packet(sock, sock->seq_num, sock->ack_num, TCP_FLAG_SYN | TCP_FLAG_ACK, NULL, 0);
            }
            break;
        
        case SYN_SENT:
            if ((tcp_pkt->flags & (TCP_FLAG_SYN | TCP_FLAG_ACK)) == (TCP_FLAG_SYN | TCP_FLAG_ACK)) {
                // Received SYN-ACK, connection is established
                sock->tcp_state = ESTABLISHED;
                sock->ack_num = seq + 1;
                
                // Send final ACK of the handshake
                tcp_send_packet(sock, ack, sock->ack_num, TCP_FLAG_ACK, NULL, 0);
            }
            break;
        
        case ESTABLISHED:
            if (tcp_pkt->flags & TCP_FLAG_FIN) {
                // Remote wants to close
                sock->tcp_state = CLOSE_WAIT;
                sock->ack_num = seq + 1;

                // Acknowledge the FIN
                tcp_send_packet(sock, ack, sock->ack_num, TCP_FLAG_ACK, NULL, 0);
            } else if (len > sizeof(tcp_packet_t)) {
                // Handle incoming data
                uint8_t* payload = data + sizeof(tcp_packet_t);
                uint32_t payload_len = len - sizeof(tcp_packet_t);
                
                // A real implementation would copy this to a receive buffer.
                // For now, we just acknowledge it.
                sock->ack_num = seq + payload_len;
                tcp_send_packet(sock, ack, sock->ack_num, TCP_FLAG_ACK, NULL, 0);
            }
            break;

        case FIN_WAIT_1:
             if (tcp_pkt->flags & TCP_FLAG_ACK) {
                // Our FIN was acknowledged
                sock->tcp_state = FIN_WAIT_2;
             }
            break;

        case FIN_WAIT_2:
            if (tcp_pkt->flags & TCP_FLAG_FIN) {
                // Remote also sent FIN
                sock->tcp_state = TIME_WAIT;
                sock->ack_num = seq + 1;
                tcp_send_packet(sock, ack, sock->ack_num, TCP_FLAG_ACK, NULL, 0);
                // A timer should be started here before moving to CLOSED
            }
            break;
        
        // ... Other states ...
        default:
            break;
    }
}
