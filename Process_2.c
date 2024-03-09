// Peer B (Run on the second PC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 256

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error initializing Winsock");
        exit(EXIT_FAILURE);
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Prepare the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to Peer A
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to Peer A");
        closesocket(client_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int N;
    int next_fibonacci;

    while (1) {
        // Send N to Peer A
        N = 2;  // Replace with the desired value of N
        ssize_t send_size = send(client_socket, (const char*)&N, sizeof(N), 0);
        if (send_size == -1) {
            perror("Error sending data");
            closesocket(client_socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Step 3: Sent N to Peer A: %d\n", N);

        // Receive the next Fibonacci value from Peer A
        ssize_t recv_size = recv(client_socket, (char*)&next_fibonacci, sizeof(next_fibonacci), 0);
        if (recv_size <= 0) {
            perror("Error receiving data");
            closesocket(client_socket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Step 4: Fibonacci(%d) received from Peer A: %d\n", N + 1, next_fibonacci);
    }

    // Close socket
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
