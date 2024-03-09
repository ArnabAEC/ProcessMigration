// Peer B (Run on the second PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return EXIT_FAILURE;
    }

    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to Peer A
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error connecting to Peer A");
        closesocket(client_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Send data to Peer A
    char data_to_send[] = "Hello from Peer B";
    int send_size = send(client_socket, data_to_send, sizeof(data_to_send), 0);
    if (send_size == SOCKET_ERROR) {
        perror("Error sending data");
        closesocket(client_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Close socket
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
