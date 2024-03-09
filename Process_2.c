// process_2.c
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "fibonacci.h"

#define PORT 12345
#define BUFFER_SIZE 256
#define PEER_A_IP "127.0.0.1"  // Hardcoded IP address of Peer A

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

    // Prompt the user to enter the value of N
    int N;
    printf("Enter the value of N: ");
    scanf("%d", &N);

    // Send the value of N to Peer A
    if (send(client_socket, (char*)&N, sizeof(N), 0) == SOCKET_ERROR) {
        perror("Error sending N to Peer A");
        closesocket(client_socket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Sent N to Peer A: %d\n", N);

    // Calculate the range of Fibonacci numbers to compute
    int start = N / 2 + 1;  // Load balancing: Peer B calculates the second half
    int end = N;

    // Calculate and send the second half of the Fibonacci sequence to Peer A
    for (int i = start; i <= end; ++i) {
        int result = fibonacci(i);
        if (send(client_socket, (char*)&result, sizeof(result), 0) == SOCKET_ERROR) {
            perror("Error sending Fibonacci value to Peer A");
            closesocket(client_socket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        printf("Sent Fibonacci(%d) to Peer A: %d\n", i, result);
    }

    // Send the end signal to indicate the completion of the sequence
    int end_signal = -1;
    if (send(client_socket, (char*)&end_signal, sizeof(end_signal), 0) == SOCKET_ERROR) {
        perror("Error sending end signal to Peer A");
    }

    // Display a message indicating completion
    printf("Sent end signal to Peer A. Calculation completed.\n");

    // Close socket
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
