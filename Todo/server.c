#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 65432
#define BUFFER_SIZE 1024
#define MAX_TASKS 100

char *tasks[MAX_TASKS];
int task_count = 0;
pthread_mutex_t lock;

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';

        // Debugging: Print the received buffer
        printf("Received buffer: %s\n", buffer);

        // Copy the buffer to avoid modifying the original buffer
        char buffer_copy[BUFFER_SIZE];
        strcpy(buffer_copy, buffer);

        // Ensure there is no trailing newline or extra spaces
        char *command = strtok(buffer_copy, " \n");

        // Debugging: Print the command extracted
        printf("Command extracted: %s\n", command);

        if (strcmp(command, "ADD") == 0) {
            char *task = strtok(NULL, "\n");

            // Debugging: Print the task being added
            printf("Task to add: %s\n", task);

            pthread_mutex_lock(&lock);
            if (task_count < MAX_TASKS) {
                tasks[task_count++] = strdup(task);
                send(client_socket, "Task added.\n", 12, 0);
            } else {
                send(client_socket, "Task list full.\n", 16, 0);
            }
            pthread_mutex_unlock(&lock);
        } else if (strcmp(command, "REMOVE") == 0) {
            char *task = strtok(NULL, "\n");

            // Debugging: Print the task being removed
            printf("Task to remove: %s\n", task);

            pthread_mutex_lock(&lock);

            int i, j;
            for (i = 0; i < task_count; i++) {
                if (strcmp(tasks[i], task) == 0) {
                    free(tasks[i]);

                    // Shift tasks left
                    for (j = i; j < task_count - 1; j++) {
                        tasks[j] = tasks[j + 1];
                    }

                    // Nullify the last task
                    tasks[task_count - 1] = NULL;
                    task_count--;

                    send(client_socket, "Task removed.\n", 14, 0);
                    break;
                }
            }
            if (i == task_count) {
                send(client_socket, "Task not found.\n", 16, 0);
            }

            pthread_mutex_unlock(&lock);
        } else if (strcmp(command, "VIEW") == 0) {
            pthread_mutex_lock(&lock);
            if (task_count == 0) {
                send(client_socket, "No tasks available.\n", 21, 0);
            } else {
                for (int i = 0; i < task_count; i++) {
                    send(client_socket, tasks[i], strlen(tasks[i]), 0);
                    send(client_socket, "\n", 1, 0);  // Ensure each task is followed by a newline
                }
            }
            pthread_mutex_unlock(&lock);
        } else {
            send(client_socket, "Invalid command.\n", 17, 0);
        }
    }
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    pthread_mutex_init(&lock, NULL);

    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) >= 0) {
        printf("Client connected.\n");
        pthread_t client_thread;
        if (pthread_create(&client_thread, NULL, (void *)handle_client, (void *)(intptr_t)client_socket) < 0) {
            perror("Thread creation failed");
            close(client_socket);
        }
        pthread_detach(client_thread);
    }

    pthread_mutex_destroy(&lock);
    close(server_socket);
    return 0;
}
