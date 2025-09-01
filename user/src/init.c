// user/src/init.c - The first user-space networking application

#include <stdint.h>

// Syscall numbers
#define SYS_SOCKET          7
#define SYS_BIND            8
#define SYS_SENDTO          9

// Network constants
#define AF_INET 2
#define SOCK_DGRAM 2

// Socket address structure must match kernel's
typedef struct {
    uint16_t sin_family;
    uint16_t sin_port;
    uint32_t sin_addr;
} sockaddr_in_t;

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

// Helper for big-endian conversion (user-space)
uint16_t htons(uint16_t val) {
    return (val >> 8) | (val << 8);
}

// Helper for string length
int strlen(const char* str) {
    int len = 0;
    while(str[len]) len++;
    return len;
}

void _start() {
    // 1. Create a UDP socket
    int sock = syscall(SYS_SOCKET, AF_INET, SOCK_DGRAM, 0, 0, 0);
    if (sock < 0) {
        // Handle error
        while(1);
    }

    // 2. Bind the socket to a local port
    sockaddr_in_t local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(1234); // Bind to port 1234
    local_addr.sin_addr = 0; // Bind to any local IP
    syscall(SYS_BIND, sock, (int)&local_addr, sizeof(local_addr), 0, 0);

    // 3. Prepare the destination address
    sockaddr_in_t dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(8000); // Send to port 8000
    // Destination IP: 8.8.8.8 (Google DNS)
    dest_addr.sin_addr = (8 << 24) | (8 << 16) | (8 << 8) | 8;
    
    // 4. Send a message
    const char* msg = "Hello from LimitlessOS!";
    syscall(SYS_SENDTO, sock, (int)msg, strlen(msg), 0, (int)&dest_addr);

    // Loop forever
    while(1);
}
