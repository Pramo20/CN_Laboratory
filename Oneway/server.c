#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>s

#define PORT 65432
#define BUFFER_SIZE 1024

int main()
{
    int server_socket, client_socket, bytes_read;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    socklen_t addr_len = sizeof(client_addr);
    char recv_buffer[BUFFER_SIZE];
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }
#ifdef SO_REUSEPORT
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt SO_REUSEPORT failed");
        exit(EXIT_FAILURE);
    }
#endif
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Waiting for a connection...\n");

    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to client.\n");

    while (1)
    {

        memset(recv_buffer, 0, BUFFER_SIZE);

        bytes_read = read(client_socket, recv_buffer, BUFFER_SIZE);
        if (bytes_read > 0)
        {
            recv_buffer[bytes_read] = '\0';
            printf("Client says: %s\n", recv_buffer);

            if (strcmp(recv_buffer, "bye") == 0)
            {
                printf("Client ended the conversation.\n");
                break;
            }
        }
        else
        {
            printf("Failed to read message from client or client disconnected\n");
            break;
        }
    }
    close(client_socket);
    close(server_socket);
    return 0;
}