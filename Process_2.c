// Peer B (Run on the second PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PEER_A_IP "127.0.0.1"  // Hardcoded IP address of Peer A
#define PORT 12345
#define BUFFER_SIZE 256

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
        return EXIT_FAILURE;
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(PEER_A_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to Peer A
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        int error_code = WSAGetLastError();
        printf("Error connecting to Peer A: %d\n", error_code);
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Connected to Peer A\n");

    while (1) {
        // Send data to Peer A
        char data_to_send[BUFFER_SIZE];
        printf("Enter command to send to Peer A: ");
        fgets(data_to_send, sizeof(data_to_send), stdin);

        int send_size = send(client_socket, data_to_send, strlen(data_to_send), 0);
        if (send_size == SOCKET_ERROR) {
            perror("Error sending data");
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Data sent to Peer A: %s\n", data_to_send);

        // Receive response from Peer A
        char response_data[BUFFER_SIZE];
        int recv_size = recv(client_socket, response_data, sizeof(response_data) - 1, 0);
        if (recv_size == SOCKET_ERROR || recv_size == 0) {
            perror("Error receiving response data");
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        response_data[recv_size] = '\0'; // Null-terminate the received data
        printf("Received response from Peer A: %s\n", response_data);
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}
