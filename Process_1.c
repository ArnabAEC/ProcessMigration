// process_1.c
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "fibonacci.c"

#define PORT 12345
#define PEER_B_IP "127.0.0.1"

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error binding socket");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == SOCKET_ERROR) {
        perror("Error listening for connections");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Peer A listening on port %d\n", PORT);

    // Accept a connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, NULL);
    if (client_socket == INVALID_SOCKET) {
        perror("Error accepting connection");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Peer A accepted a connection from Peer B\n");

    // Receive the value of N from Peer B
    int N;
    if (recv(client_socket, (char*)&N, sizeof(N), 0) <= 0) {
        perror("Error receiving N from Peer B");
        closesocket(server_socket);
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Received N from Peer B: %d\n", N);

    // Collaborative Fibonacci calculation
    for (int i = 2; i <= N; ++i) {
        int result = fibonacci(i);
        if (send(client_socket, (char*)&result, sizeof(result), 0) == SOCKET_ERROR) {
            perror("Error sending Fibonacci value to Peer B");
            closesocket(server_socket);
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Sent Fibonacci(%d) to Peer B: %d\n", i, result);

        // Receive the next value from Peer B
        int recv_result = recv(client_socket, (char*)&result, sizeof(result), 0);
        if (recv_result <= 0) {
            if (recv_result == 0) {
                printf("Peer B closed the connection\n");
            } else {
                perror("Error receiving data from Peer B");
            }
            break;  // End of sequence received or error occurred
        }

        printf("Fibonacci(%d) received from Peer B: %d\n", i + 1, result);
    }

    // Close sockets
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
