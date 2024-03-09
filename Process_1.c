// Peer A (Run on the first PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUFFER_SIZE 256
#define PEER_B_IP "127.0.0.1"  // Hardcoded IP address of Peer B

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_size = sizeof(struct sockaddr_in);

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
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size);
    if (client_socket == INVALID_SOCKET) {
        perror("Error accepting connection");
        closesocket(server_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Peer A accepted a connection from Peer B\n");

    while (1) {
        // Receive data from Peer B
        char received_data[BUFFER_SIZE];
        int recv_size = recv(client_socket, received_data, sizeof(received_data) - 1, 0);
        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            perror("Error receiving data");
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        received_data[recv_size] = '\0'; // Null-terminate the received data
        printf("Received data from Peer B: %s\n", received_data);

        // Additional processing of data (replace with your actual processing logic)
        // For example, you can send a response back to Peer B.
        char response_data[BUFFER_SIZE];
        printf("Enter response to send to Peer B: ");
        fgets(response_data, sizeof(response_data), stdin);

        // Send response to Peer B
        int send_size = send(client_socket, response_data, strlen(response_data), 0);
        if (send_size == SOCKET_ERROR) {
            perror("Error sending response to Peer B");
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Response sent to Peer B: %s\n", response_data);
    }

    // Close the server socket (This part will not be reached in the loop)
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
