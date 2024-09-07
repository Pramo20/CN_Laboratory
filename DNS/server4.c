#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define MAXLINE 1000

int getipaddr(const char *key, char *value) {
    char filename[] = "data.txt";
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char file_key[256];
        char file_value[256];
        sscanf(line, "%s %s", file_key, file_value);
        if (strcmp(file_key, key) == 0) {
            strcpy(value, file_value);
            fclose(file);
            return 0;
        }
    }

    strcpy(value, "IP address not found");
    fclose(file);
    return 0;
}

int main() {
    char buffer[MAXLINE];
    char message[MAXLINE];
    int listenfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    // Create a datagram socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (listenfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Clear and set server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(listenfd);
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);

    while (1) {
        // Receive the domain name from the client
        int n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
        if (n < 0) {
            perror("Receive failed");
            close(listenfd);
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';  // Null-terminate the received message

        // Check for "end" command to terminate the server
        if (strcmp(buffer, "end") == 0) {
            printf("Server: Received 'end' command. Shutting down.\n");
            break;
        }

        // Look up the IP address in the file
        getipaddr(buffer, message);

        // Send the IP address or "not found" message back to the client
        sendto(listenfd, message, strlen(message), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    }

    // Close the socket
    close(listenfd);
    return 0;
}
