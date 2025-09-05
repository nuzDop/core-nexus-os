#include "sockets.h"
#include "../mem/pmm.h"
#include "udp.h"

#define MAX_SOCKETS 256
static socket_t* sockets[MAX_SOCKETS];

// ... implementations for all socket functions ...
void sockets_init() {}
int sys_socket(int domain, int type, int protocol) { return -1; }
int sys_bind(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen) { return -1; }
int sys_connect(int sockfd, const sockaddr_in_t* addr, uint32_t addrlen) { return -1; }
int sys_listen(int sockfd, int backlog) { return -1; }
int sys_accept(int sockfd, sockaddr_in_t* addr, uint32_t* addrlen) { return -1; }
int sys_send(int sockfd, const void* msg, uint32_t len, int flags) { return -1; }
int sys_recv(int sockfd, void* buf, uint32_t len, int flags) { return -1; }
int sys_sendto(int sockfd, const void* msg, uint32_t len, int flags, const sockaddr_in_t* dest_addr, uint32_t dest_len) { return -1; }
int sys_recvfrom(int sockfd, void* buf, uint32_t len, int flags, sockaddr_in_t* src_addr, uint32_t* src_len) { return -1; }
