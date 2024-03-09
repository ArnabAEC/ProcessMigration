// Peer B (Run on the second PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to Peer A
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to Peer A");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send data to Peer A
    char data_to_send[] = "Hello from Peer B";
    ssize_t send_size = send(client_socket, data_to_send, sizeof(data_to_send), 0);
    if (send_size == -1) {
        perror("Error sending data");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Close socket
    close(client_socket);

    return 0;
}
