#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "net.h"

// TCP Header Flags
#define TCP_FLAG_FIN (1 << 0)
#define TCP_FLAG_SYN (1 << 1)
#define TCP_FLAG_RST (1 << 2)
#define TCP_FLAG_PSH (1 << 3)
#define TCP_FLAG_ACK (1 << 4)
#define TCP_FLAG_URG (1 << 5)

// TCP Connection States
typedef enum {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT
} tcp_state_t;

typedef struct {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_num;
    uint32_t ack_num;
    uint8_t  offset_reserved; // 4 bits offset, 4 bits reserved
    uint8_t  flags;
    uint16_t window_size;
    uint16_t checksum;
    uint16_t urgent_ptr;
} __attribute__((packed)) tcp_packet_t;

// TCP Control Block - holds all state for a single connection
typedef struct {
    tcp_state_t state;
    
    // Sequence Variables
    uint32_t snd_una; // Send Unacknowledged
    uint32_t snd_nxt; // Send Next
    uint32_t snd_wnd; // Send Window (from remote)
    uint32_t rcv_nxt; // Receive Next
    uint32_t rcv_wnd; // Receive Window (our available buffer space)

    // Retransmission Timer
    uint32_t rto;     // Retransmission Timeout value
    uint32_t rtx_timer; // Current timer countdown

    // Congestion Control (TCP Reno)
    uint32_t cwnd;    // Congestion Window
    uint32_t ssthresh; // Slow Start Threshold
    
    // Buffers (a real implementation would use dynamic ring buffers)
    uint8_t* send_buffer;
    uint32_t send_buffer_size;
    uint8_t* recv_buffer;
    uint32_t recv_buffer_size;

} tcp_control_block_t;

struct socket; // Forward declaration from sockets.h

void tcp_init();
void tcp_handle_packet(net_device_t* dev, uint32_t src_ip, uint8_t* data, uint32_t len);
void tcp_send(struct socket* sock, const uint8_t* data, uint32_t len, bool push);
void tcp_connect(struct socket* sock);
void tcp_close(struct socket* sock);
void tcp_timer_tick(); // Called by the system timer

#endif
