#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024

void send_command(int socket, const char *command) {
    send(socket, command, strlen(command), 0);
    char buffer[BUFFER_SIZE];
    int read_size;
    while ((read_size = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("%s", buffer);
        if (read_size < BUFFER_SIZE)
            break;
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char command[BUFFER_SIZE];
    int choice;

    while (1) {
        printf("\n1. Add Task\n2. Remove Task\n3. View Tasks\n4. Exit\nChoose an option: ");
        scanf("%d", &choice);
        getchar();  // Consume newline character

        switch (choice) {
            case 1:
                printf("Enter task to add: ");
                fgets(command, BUFFER_SIZE, stdin);
                command[strcspn(command, "\n")] = 0; // Remove newline character
                char add_command[BUFFER_SIZE];
                snprintf(add_command, sizeof(add_command), "ADD %s\n", command);
                send_command(client_socket, add_command);
                break;
            case 2:
                printf("Enter task to remove: ");
                fgets(command, BUFFER_SIZE, stdin);
                command[strcspn(command, "\n")] = 0; // Remove newline character
                char remove_command[BUFFER_SIZE];
                snprintf(remove_command, sizeof(remove_command), "REMOVE %s\n", command);
                send_command(client_socket, remove_command);
                break;
            case 3:
                send_command(client_socket, "VIEW\n");
                break;
            case 4:
                close(client_socket);
                exit(EXIT_SUCCESS);
            default:
                printf("Invalid choice.\n");
        }
    }

    close(client_socket);
    return 0;
}

