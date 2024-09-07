#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 65432
#define MAX_BUFFER 1024

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <server IP>\n", argv[0]);
        return -1;
    }

    struct sockaddr_in server_addr;
    int client_socket = 0;
    char user_message[MAX_BUFFER];

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    memset(&server_addr, '0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("\nConnection Failed\n");
        return -1;
    }

    while (1)
    {

        printf("Enter the message to send: ");
        if (fgets(user_message, sizeof(user_message), stdin) == NULL)
        {
            printf("\nFailed to read input\n");
            break;
        }

        user_message[strcspn(user_message, "\n")] = '\0';

        if (send(client_socket, user_message, strlen(user_message), 0) == -1)
        {
            printf("Failed to send message\n");
            break;
        }
        printf("Message sent\n");

        if (strcmp(user_message, "bye") == 0)
        {
            printf("You ended the conversation.\n");
            break;
        }
    }

    close(client_socket);

    return 0;
}
