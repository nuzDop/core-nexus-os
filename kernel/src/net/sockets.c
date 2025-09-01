#include "sockets.h"
#include "../mem/pmm.h"
#include "udp.h" // For udp_send_packet

#define MAX_SOCKETS 256
static socket_t* sockets[MAX_SOCKETS];

void sockets_init() {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i] = 0;
    }
}

int sys_socket(int domain, int type, int protocol) {
    for (int i = 1; i < MAX_SOCKETS; i++) { // Start from 1, 0 is reserved
        if (sockets[i] == 0) {
            socket_t* new_sock = (socket_t*)pmm_alloc_page();
            new_sock->domain = domain;
            new_sock->type = type;
            new_sock->protocol = protocol;
            sockets[i] = new_sock;
            return i; // Return the socket descriptor (index)
        }
    }
    return -1;
}

int sys_bind(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen) {
    if (sockfd <= 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd]) {
        return -1; // Invalid socket
    }
    // A real implementation would check address validity and availability
    sockets[sockfd]->local_addr = *addr;
    return 0; // Success
}

int sys_sendto(int sockfd, const void* msg, uint32_t len, int flags, const sockaddr_in_t* dest_addr, uint32_t dest_len) {
    if (sockfd <= 0 || sockfd >= MAX_SOCKETS || !sockets[sockfd]) {
        return -1;
    }
    
    socket_t* sock = sockets[sockfd];

    // Assuming UDP for now since that's all we support
    if (sock->type == SOCK_DGRAM) {
        udp_send_packet(dest_addr->sin_addr, dest_addr->sin_port, sock->local_addr.sin_port, (void*)msg, len);
        return len; // Return number of bytes sent
    }

    return -1; // Unsupported socket type
}

int sys_recvfrom(int sockfd, void* buf, uint32_t len, int flags, sockaddr_in_t* src_addr, uint32_t* src_len) {
    // Placeholder for receiving data
    return -1;
}
