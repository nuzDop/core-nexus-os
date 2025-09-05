/* kernel/src/net/sockets.c */

#include "sockets.h"
#include "../mem/pmm.h"
#include "../proc/task.h"
#include "udp.h"
#include "tcp.h"
#include "../lib/string.h"

#define MAX_SOCKETS 256
static socket_t* sockets[MAX_SOCKETS];

void sockets_init() {
    memset(sockets, 0, sizeof(socket_t*) * MAX_SOCKETS);
    print("Socket layer initialized.\n");
}

// Finds the next available socket descriptor
static int find_free_socket_fd() {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i] == NULL) {
            return i;
        }
    }
    return -1;
}

// Finds a socket matching the full 4-tuple, used by TCP/UDP handlers
socket_t* find_socket_by_addr(uint32_t rip, uint16_t rport, uint32_t lip, uint16_t lport) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        socket_t* sock = sockets[i];
        if (sock &&
            sock->remote_addr.sin_addr == rip &&
            sock->remote_addr.sin_port == rport &&
            sock->local_addr.sin_addr == lip &&
            sock->local_addr.sin_port == lport)
        {
            return sock;
        }
    }
    return NULL;
}

int sys_socket(int domain, int type, int protocol) {
    if (domain != AF_INET) return -1;

    int fd = find_free_socket_fd();
    if (fd == -1) return -1; // No free sockets

    socket_t* sock = (socket_t*)pmm_alloc_page();
    memset(sock, 0, sizeof(socket_t));
    sock->domain = domain;
    sock->type = type;
    sock->protocol = protocol;
    sock->tcp_state = CLOSED;

    sockets[fd] = sock;
    return fd;
}

int sys_bind(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen) {
    if (sockfd < 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd] || !addr) {
        return -1;
    }
    
    socket_t* sock = sockets[sockfd];
    memcpy(&sock->local_addr, addr, sizeof(sockaddr_in_t));

    return 0;
}

int sys_connect(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen) {
    if (sockfd < 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd] || !addr) {
        return -1;
    }

    socket_t* sock = sockets[sockfd];
    memcpy(&sock->remote_addr, addr, sizeof(sockaddr_in_t));

    // A real implementation would pick an ephemeral source port if not bound
    if (sock->local_addr.sin_port == 0) {
        sock->local_addr.sin_port = htons(49152 + (sockfd % (65535-49152)));
    }

    // Initiate TCP Handshake
    sock->tcp_state = SYN_SENT;
    sock->seq_num = 0; // A real implementation uses a random ISN
    tcp_send_packet(sock, sock->seq_num, 0, TCP_FLAG_SYN, NULL, 0);

    // Block until the connection is established or fails
    // A real implementation would use a wait queue and scheduler sleep.
    while(sock->tcp_state == SYN_SENT) {
        // Yield or sleep
    }

    if (sock->tcp_state == ESTABLISHED) {
        return 0;
    }

    return -1;
}

int sys_listen(int sockfd, int backlog) {
    if (sockfd < 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd]) {
        return -1;
    }
    sockets[sockfd]->tcp_state = LISTEN;
    // A real implementation would initialize the backlog queue here.
    return 0;
}

int sys_accept(int sockfd, sockaddr_in_t* addr, uint32_t* addrlen) {
    // This is a complex function requiring a backlog queue. Stubbed for now.
    return -1;
}

int sys_send(int sockfd, const void* msg, uint32_t len, int flags) {
    if (sockfd < 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd]) {
        return -1;
    }
    socket_t* sock = sockets[sockfd];
    if (sock->tcp_state != ESTABLISHED) return -1;
    
    // A real implementation would buffer this data and handle segmentation.
    tcp_send_packet(sock, sock->seq_num, sock->ack_num, TCP_FLAG_PSH | TCP_FLAG_ACK, (uint8_t*)msg, len);
    sock->seq_num += len;
    
    // Block waiting for the ACK (very simplified)
    // while (/* last ack not received */) { sleep(); }

    return len;
}

int sys_recv(int sockfd, void* buf, uint32_t len, int flags) {
    if (sockfd < 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd]) {
        return -1;
    }
    // This is complex, requires a receive buffer that tcp_handle_packet fills.
    // Block until data is available in the buffer.
    return -1;
}

int sys_sendto(int sockfd, const void* msg, uint32_t len, int flags, const sockaddr_in_t* dest_addr, uint32_t dest_len) {
    // Primarily for UDP
    return -1;
}

int sys_recvfrom(int sockfd, void* buf, uint32_t len, int flags, sockaddr_in_t* src_addr, uint32_t* src_len) {
    // Primarily for UDP
    return -1;
}
