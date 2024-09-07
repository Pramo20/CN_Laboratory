// SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[1024];
    socklen_t len;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Socket creation failed");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    // Server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("Bind failed");
    }

    while (1) {
        len = sizeof(cli_addr);
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cli_addr, &len);
        if (n < 0) {
            error("Receive failed");
        }
        buffer[n] = '\0';
        printf("Received '%s'\n", buffer);

        // Prompt user to enter ACK manually
        char ack[10];
        printf("Type 'ACK' to acknowledge '%s': ", buffer);
        fgets(ack, sizeof(ack), stdin);
        ack[strcspn(ack, "\n")] = '\0';  // Remove newline character

        if (strcmp(ack, "ACK") == 0) {
            sendto(sockfd, "ACK", strlen("ACK"), 0, (const struct sockaddr *)&cli_addr, len);
            printf("Sent ACK for '%s'\n", buffer);
        } else {
            printf("No ACK sent for '%s'\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}

