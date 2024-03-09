// Peer A (Run on the first PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "fibonacci.h"

#define PORT 12345
#define BUFFER_SIZE 256
#define SERVER_IP "10.145.21.58"  // Replace with the IP of Peer B

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(struct sockaddr_in);

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error initializing Winsock");
        exit(EXIT_FAILURE);
    }

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Peer A listening on port %d\n", PORT);

    // Accept a connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket == -1) {
        perror("Error accepting connection");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int N;
    int next_fibonacci;

    while (1) {
        // Receive N from Peer B
        ssize_t recv_size = recv(client_socket, (char*)&N, sizeof(N), 0);
        if (recv_size <= 0) {
            perror("Error receiving data");
            closesocket(server_socket);
            closesocket(client_socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Step 1: Received N from Peer B: %d\n", N);

        // Calculate the next Fibonacci value
        next_fibonacci = fibonacci(N + 1);

        // Send the next Fibonacci value to Peer B
        ssize_t send_size = send(client_socket, (const char*)&next_fibonacci, sizeof(next_fibonacci), 0);
        if (send_size == -1) {
            perror("Error sending data");
            closesocket(server_socket);
            closesocket(client_socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Step 2: Sent Fibonacci(%d) to Peer B: %d\n", N + 1, next_fibonacci);
    }

    // Close sockets
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
