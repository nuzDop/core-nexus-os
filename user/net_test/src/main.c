#include <stdint.h>

#define SYS_PRINT           1
#define SYS_SOCKET          7
#define SYS_CONNECT         20
#define SYS_SEND            23
#define SYS_RECV            24

#define AF_INET 2
#define SOCK_STREAM 1

typedef struct {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
} sockaddr_in_t;

int syscall(int num, int p1, int p2, int p3, int p4, int p5);
uint16_t htons(uint16_t val) { return (val >> 8) | (val << 8); }
int strlen(const char* str) { int len = 0; while(str[len]) len++; return len; }

void _start() {
    syscall(SYS_PRINT, (int)"Net Test: Attempting TCP connection...\n", 0,0,0,0);
    int sock = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0, 0, 0);

    sockaddr_in_t dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(80);
    dest_addr.sin_addr = (93 << 24) | (184 << 16) | (216 << 8) | 34;
    
    if (syscall(SYS_CONNECT, sock, (int)&dest_addr, sizeof(dest_addr), 0, 0) < 0) {
        syscall(SYS_PRINT, (int)"Connection failed.\n", 0,0,0,0);
        while(1);
    }
    
    const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    syscall(SYS_SEND, sock, (int)request, strlen(request), 0, 0);

    char buffer[2048];
    int bytes_received = syscall(SYS_RECV, sock, (int)buffer, 2047, 0, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        syscall(SYS_PRINT, (int)buffer, 0,0,0,0);
    }
    while(1);
}
