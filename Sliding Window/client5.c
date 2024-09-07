// CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TIMEOUT 10 // Timeout in seconds
#define PORT 8080

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, num_frames, frame_no = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    struct timeval timeout;

    // Ask for number of frames
    printf("Enter the number of frames to send: ");
    scanf("%d", &num_frames);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        error("Socket creation failed");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    // Server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Assuming the server is running on the same machine

    // Set timeout for receiving
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        error("Error setting timeout");
    }

    while (frame_no < num_frames) {
        sprintf(buffer, "Frame %d", frame_no + 1);

        // Send frame
        printf("Sending '%s'\n", buffer);
        sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&serv_addr, sizeof(serv_addr));

        // Wait for acknowledgment
        int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received '%s'. Moving to the next frame.\n", buffer);
            frame_no++;
        } else {
            printf("Timeout, no ACK received for 'Frame %d'. Resending...\n", frame_no + 1);
        }
    }

    close(sockfd);
    return 0;
}
