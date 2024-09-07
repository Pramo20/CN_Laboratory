#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define MAXLINE 1000

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Clear and set server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    while (1) {
        // Input the domain name
        printf("Enter the domain name: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove the newline character

        // Send the domain name to the server
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr*)&servaddr, sizeof(servaddr));

        // Check for "end" command to terminate the client
        if (strcmp(buffer, "end") == 0) {
            printf("Client: 'end' command received. Exiting.\n");
            break;
        }

        // Receive the IP address from the server
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
        buffer[n] = '\0'; // Null-terminate the received message

        // Display the IP address or error message
        printf("IP Address: %s\n", buffer);
    }

    // Close the socket
    close(sockfd);
    return 0;
}
