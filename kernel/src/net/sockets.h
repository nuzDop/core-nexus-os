#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdint.h>
#include "tcp.h"

#define AF_INET 2
#define SOCK_STREAM 1
#define SOCK_DGRAM 2

typedef struct {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
} sockaddr_in_t;

typedef struct {
    int domain;
    int type;
    int protocol;
    sockaddr_in_t local_addr;
    sockaddr_in_t remote_addr;

    // TCP specific state
    tcp_state_t tcp_state;
    // ... other TCB fields
} socket_t;

void sockets_init();
int sys_socket(int domain, int type, int protocol);
int sys_bind(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen);
int sys_connect(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen);
int sys_listen(int sockfd, int backlog);
int sys_accept(int sockfd, sockaddr_in_t* addr, uint32_t* addrlen);
int sys_send(int sockfd, const void* msg, uint32_t len, int flags);
int sys_recv(int sockfd, void* buf, uint32_t len, int flags);
int sys_sendto(int sockfd, const void* msg, uint32_t len, int flags, const sockaddr_in_t* dest_addr, uint32_t dest_len);
int sys_recvfrom(int sockfd, void* buf, uint32_t len, int flags, sockaddr_in_t* src_addr, uint32_t* src_len);

#endif
