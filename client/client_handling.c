#include "headers.h" // Replace with your actual header file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define MAX_TOKENS 10
#define MAX_CLIENTS 100

// Global array to manage client sockets
// int client_socket[MAX_CLIENTS];

// // Function to handle client communication
// int handle_client(int client_sock) {
//     st_response response; // Assuming this structure is defined in "headers.h"
//     int bytes_received = recv(client_sock, &response, sizeof(st_response), 0);
//     if (bytes_received < 0) {
//         perror("Failed to receive data");
//         close(client_sock); // Close the socket on error
//         return -1;            // Indicate socket is closed
//     } else if (bytes_received == 0) {
//         printf("Client socket %d closed the connection.\n", client_sock);
//         close(client_sock); // Close the socket if client disconnects
//         return -1;            // Indicate socket is closed
//     }

//     if (response.response_type == ACK) {
//         printf(GREEN("Creation of Directory or File successful.\n"));
//     } else {
//         printf(RED("Creation of Directory or File not successful: %s\n"), response.message);
//     }
//     printf("%s\n",response.message);

//     return 0; // Indicate socket is still valid
// }

// // Thread function to monitor client sockets
// void *monitor_sockets(void *arg) {
//     int *client_sockets = (int *)arg;
//     fd_set read_fds;

//     while (1) {
//         FD_ZERO(&read_fds); // Clear the read_fds set
//         int max_fd = -1;

//         // Add each client socket to the fd_set
//         for (int i = 0; i < MAX_CLIENTS; i++) {
//             if (client_sockets[i] != -1) { // Only consider valid sockets
//                 FD_SET(client_sockets[i], &read_fds);
//                 if (client_sockets[i] > max_fd) {
//                     max_fd = client_sockets[i]; // Track the highest fd
//                 }
//             }
//         }

//         if (max_fd == -1) {
//             // No active sockets, sleep briefly and continue
//             usleep(100000); // Sleep for 100ms
//             continue;
//         }

//         // Wait for activity on any of the client sockets
//         int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
//         if (activity < 0) {
//             perror("Select failed");
//             break;
//         }

//         // Check each socket to see if it is ready to read
//         for (int i = 0; i < MAX_CLIENTS; i++) {
//             if (client_sockets[i] != -1 && FD_ISSET(client_sockets[i], &read_fds)) {
//                 // If socket is ready to read, handle it
//                 if (handle_client(client_sockets[i]) == -1) {
//                     // If the socket is closed, remove dit from the list
//                     printf("Socket %d closed, removing from client list.\n", client_sockets[i]);
//                     client_sockets[i] = -1; // Mark as invalid
//                 }
//             }
//         }
//     }
//     return NULL;
// }

// Main function
int main() {
    printf(BLUE("For input-related doubts, type MAN\n"));

    char input[3000];
    char *tokens[MAX_TOKENS];

    // Initialize client_sockets
    // for (int i = 0; i < MAX_CLIENTS; i++){
    //     client_socket[i] = -1;
    // }

    // Create monitor_sockets thread once
    // pthread_t handle_ack;
    // pthread_create(&handle_ack, NULL, monitor_sockets, (void *)client_socket);

    while (1) {
        printf(YELLOW_COLOR"\n------- Enter operation you want to do -------\n"RESET_COLOR);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        // Tokenize and store in the tokens array
        int token_count = 0;
        char *token = strtok(input, " ");
        while (token != NULL && token_count < MAX_TOKENS) {
            tokens[token_count++] = token;
            token = strtok(NULL, " ");
        }

        if (token_count == 0) {
            printf(RED("No operation entered. Try again.\n"));
            continue;
        }

        // Process input commands
        if (strcmp("READ", tokens[0]) == 0 && token_count == 2) {
            reading_operation(tokens[1]); // Implement this function
        } else if (strcmp("WRITE", tokens[0]) == 0 && token_count == 2) {
            writing_append_operation(tokens[1], 1); // Implement this function
        } else if (strcmp("CREATE", tokens[0]) == 0 && token_count == 4) {
            if (strcmp("FOLDER", tokens[1]) == 0) {
                create_operation(tokens[2], tokens[3], CREATE_FOLDER); // Implement this function
            } else if (strcmp("FILE", tokens[1]) == 0) {
                create_operation(tokens[2], tokens[3], CREATE_FILE); // Implement this function
            }
        } else if (strcmp("DELETE", tokens[0]) == 0 && token_count == 4) {
            if (strcmp("FOLDER", tokens[1]) == 0) {
                delete_operation(tokens[2], tokens[3], DELETE_FOLDER); // Implement this function
            } else if (strcmp("FILE", tokens[1]) == 0) {
                delete_operation(tokens[2], tokens[3], DELETE_FILE); // Implement this function
            }
        } else if (strcmp("COPY", tokens[0]) == 0 && token_count == 4) {
            if (strcmp("FOLDER", tokens[1]) == 0) {
                copy_operation(COPY_FOLDER, tokens[2], tokens[3]); // Implement this function
            } else {
                copy_operation(COPY_FILE, tokens[2], tokens[3]); // Implement this function
            }
        } else if (strcmp("STREAM", tokens[0]) == 0 && token_count == 2) {
            stream(tokens[1]); // Implement this function
        } else if (strcmp("INFO", tokens[0]) == 0 && token_count == 2) {
            info(tokens[1]); // Implement this function
        } else if (strcmp("LIST", tokens[0]) == 0) {
            list(); // Implement this function
        } else if (strcmp("EXIT", tokens[0]) == 0) {
            break;
        } else {
            printf(RED_COLOR "Invalid operation. Try again.\n" RESET_COLOR);
        }
    }

    // Terminate monitor thread and clean up
    // pthread_cancel(handle_ack);
    // pthread_join(handle_ack, NULL);

    return 0;
}
