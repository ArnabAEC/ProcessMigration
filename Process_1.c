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
        // Additional processing of data (replace with your actual processing logic)
        // For example, you can send a message to Peer B.
        char message_to_send[BUFFER_SIZE];
        printf("Enter message to send to Peer B: ");
        fgets(message_to_send, sizeof(message_to_send), stdin);

        // Create a socket to connect to Peer B
        int peer_b_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (peer_b_socket == INVALID_SOCKET) {
            perror("Error creating socket for Peer B");
            closesocket(server_socket);
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        struct sockaddr_in peer_b_addr;
        peer_b_addr.sin_family = AF_INET;
        peer_b_addr.sin_addr.s_addr = inet_addr(PEER_B_IP);
        peer_b_addr.sin_port = htons(PORT); // Assuming the same port for simplicity

        // Connect to Peer B
        if (connect(peer_b_socket, (struct sockaddr*)&peer_b_addr, sizeof(peer_b_addr)) == SOCKET_ERROR) {
            perror("Error connecting to Peer B");
            closesocket(peer_b_socket);
            continue; // Continue listening for the next connection
        }

        printf("Connected to Peer B\n");

        // Send message to Peer B
        int send_size = send(peer_b_socket, message_to_send, strlen(message_to_send), 0);
        if (send_size == SOCKET_ERROR) {
            perror("Error sending message to Peer B");
            closesocket(peer_b_socket);
            continue; // Continue listening for the next connection
        }

        printf("Message sent to Peer B: %s\n", message_to_send);

        // Close the socket (Peer B)
        closesocket(peer_b_socket);
    }

    // Close the server socket (This part will not be reached in the loop)
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
