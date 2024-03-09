// Peer B (Run on the second PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
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
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    while (1) {
        // Connect to Peer A
        if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            printf("Error connecting to Peer A: %d\n", error_code);
            Sleep(1000); // Sleep for a moment before retrying
            continue;
        }

        printf("Connected to Peer A\n");

        // Send data to Peer A
        char data_to_send[BUFFER_SIZE];
        printf("Enter command to send to Peer A: ");
        fgets(data_to_send, sizeof(data_to_send), stdin);

        int send_size = send(client_socket, data_to_send, strlen(data_to_send), 0);
        if (send_size == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            printf("Error sending data: %d\n", error_code);
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Data sent to Peer A: %s\n", data_to_send);

        // Close the socket (Peer A)
        closesocket(client_socket);
        Sleep(1000); // Sleep for a moment before retrying
    }

    WSACleanup();

    return 0;
}
