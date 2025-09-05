#include <stdint.h>

// Syscall numbers
#define SYS_SOCKET          7
#define SYS_CONNECT         20
#define SYS_SEND            23
#define SYS_RECV            24
#define SYS_PRINT           1

// Network constants
#define AF_INET 2
#define SOCK_STREAM 1

typedef struct {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
} sockaddr_in_t;

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

uint16_t htons(uint16_t val) {
    return (val >> 8) | (val << 8);
}

int strlen(const char* str) {
    int len = 0;
    while(str[len]) len++;
    return len;
}

void _start() {
    syscall(SYS_PRINT, (int)"Init: Testing TCP Stack...\n", 0,0,0,0);

    // 1. Create a TCP socket
    int sock = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0, 0, 0);
    if (sock < 0) {
        syscall(SYS_PRINT, (int)"Socket creation failed.\n", 0,0,0,0);
        while(1);
    }

    // 2. Prepare destination address (e.g., example.com at 93.184.216.34)
    sockaddr_in_t dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(80); // HTTP port
    dest_addr.sin_addr = (93 << 24) | (184 << 16) | (216 << 8) | 34;
    
    // 3. Connect to the server
    if (syscall(SYS_CONNECT, sock, (int)&dest_addr, sizeof(dest_addr), 0, 0) < 0) {
        syscall(SYS_PRINT, (int)"Connection failed.\n", 0,0,0,0);
        while(1);
    }
    
    // 4. Send an HTTP GET request
    const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    syscall(SYS_SEND, sock, (int)request, strlen(request), 0, 0);

    // 5. Receive the response
    char buffer[1024];
    int bytes_received = syscall(SYS_RECV, sock, (int)buffer, 1023, 0, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        syscall(SYS_PRINT, (int)buffer, 0,0,0,0);
    }

    while(1);
}
