#include "headers.h"

typedef struct
{
    int socket;
} thread_arg_t;

// Thread handler to listen for acknowledgment
void *thread_handler(void *arg)
{
    thread_arg_t *t_arg = (thread_arg_t *)arg;
    int client_socket = t_arg->socket;
    free(t_arg); // Free allocated memory for thread arguments

    st_request response;
    while (1)
    {
        ssize_t bytes_received = recv(client_socket, &response, sizeof(st_request), 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf(RED_COLOR "Connection closed by server.\n" RESET_COLOR);
            }
            else
            {
                perror("Error while receiving acknowledgment");
            }
            break; // Exit the loop on error or connection closure
        }

        if (response.request_type == ACK)
        {
            printf(GREEN_COLOR "Acknowledgment received: %s\n" RESET_COLOR, response.data);
            break;
        }
        else
        {
            printf(YELLOW_COLOR "Unexpected response: %s\n" RESET_COLOR, response.data);
        }
    }

    close(client_socket); // Close the socket in the thread
    pthread_exit(NULL);   // Exit the thread
}

void communicate_with_ss_write(char *ipaddress, int port, char *path)
{
    int client_socket = connect_with_ss(ipaddress, port);
    if (client_socket < 0)
    {
        perror("Connection failed");
        return;
    }

    st_request packet;
    packet.request_type = WRITE_REQ;
    strcpy(packet.src_path, path);

    printf(BLUE_COLOR "ENTER INPUT TO BE WRITTEN:\n" RESET_COLOR);
    char input;
    int dataread = 0;

    while ((input = fgetc(stdin)) != '\n' && input != EOF)
    {
        if (dataread >= MAX_DATA_LENGTH - 1)
        {
            // Send the current packet
            if (send(client_socket, &packet, sizeof(st_request), 0) == -1)
            {
                perror("Send failed");
                close(client_socket);
                return;
            }
            dataread = 0; // Reset dataread after sending
        }

        // Add character to buffer
        packet.data[dataread++] = input;
        packet.data[dataread] = '\0'; // Null-terminate
    }

    // Send any remaining data
    if (dataread > 0)
    {
        if (send(client_socket, &packet, sizeof(st_request), 0) == -1)
        {
            perror("Send failed");
            close(client_socket);
            return;
        }
    }

    // Create a thread to listen for acknowledgment
    // pthread_t ack_thread;
    // thread_arg_t *t_arg = malloc(sizeof(thread_arg_t));
    // if (t_arg == NULL)
    // {
    //     perror("Memory allocation for thread arguments failed");
    //     close(client_socket);
    //     return;
    // }
    st_request response;
    ssize_t bytes_received = recv(client_socket, &response, sizeof(st_request), 0);
    if (bytes_received <= 0)
    {
        // close(client_socket);
        perror("Receiving acknowledgment failed");
    }
    else if (response.request_type == ACK)
    {
        // close(client_socket);
        printf(GREEN_COLOR "Write operation completed successfully.\n" RESET_COLOR);
    }
    else
    {
        printf(RED_COLOR "Failed to Write Data: %s" RESET_COLOR, response.data);
    }
    // t_arg->socket = client_socket;
    // if (pthread_create(&ack_thread, NULL, thread_handler, t_arg) != 0)
    // {
    //     perror("Failed to create acknowledgment thread");
    //     free(t_arg); // Free memory if thread creation fails
    //     close(client_socket);
    //     return;
    // }

    // // Detach the thread to allow it to clean up on its own
    // pthread_detach(ack_thread);

    // printf(GREEN_COLOR "Write operation completed. Waiting for acknowledgment in a separate thread.\n" RESET_COLOR);
}

void writing_append_operation(char *path, int f)
{
    // ack* client = connect_with_ns_write();
    int client_socket = connect_with_ns();
    st_request writerpacket;
    writerpacket.request_type = WRITE_REQ;
    strcpy(writerpacket.src_path, path);
    // strcpy(client->path,path);
    int bytes_sent = send(client_socket, &writerpacket, sizeof(st_request), 0);
    if (bytes_sent == -1)
    {
        perror("Send failed");
    }
    st_response response;
    int bytes_received = recv(client_socket, &response, sizeof(st_response), 0);
    int port;
    char *ipaddress = (char *)malloc(16 * sizeof(char));
    if (bytes_received == -1)
    {
        perror("Receive failed");
    }
    else
    {
        if (response.response_type == FILE_NOT_FOUND)
        {
            printf(RED("File Not Found \n")); // Error Not Found File
            return;
        }
        else if (response.response_type == INVALID_FILETYPE)
        {
            printf(RED("Invalid file type\n"));
            close(client_socket);
            return;
        }
        else
        {
            strcpy(ipaddress, response.IP_Addr);
            port = response.Port_No;
        }
    }
    close(client_socket);

    communicate_with_ss_write(ipaddress, port, path);
    // printf(GREEN_COLOR"------Write/Append Completed------\n"RESET_COLOR);
    return;
}
