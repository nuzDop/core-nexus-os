#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "10.0.2.2"
#define SERVER_PORT 8080
#define MESSAGE "Hello from client"

int main() {
    int sock;
    struct sockaddr_in server;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Send message to server
    if (send(sock, MESSAGE, strlen(MESSAGE), 0) < 0) {
        perror("Send failed");
        return 1;
    }

    printf("Message sent to server\n");

    // Receive response from server
    if (read(sock, buffer, 1024) < 0) {
        perror("Read failed");
        return 1;
    }

    printf("Server response: %s\n", buffer);

    close(sock);

    return 0;
}
