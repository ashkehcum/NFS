#include "headers.h"

void communicate_with_ss(char *ipaddress, int port, char *path) {
    printf("Connecting to server at %s:%d\n", ipaddress, port);

    int client_socket = connect_with_ss(ipaddress, port);
    if (client_socket < 0) {
        perror("Connection failed");
        return;
    }

    st_request readerpacket;
    readerpacket.request_type = READ_REQ;
    strcpy(readerpacket.src_path, path);
    readerpacket.socket = client_socket;

    if (send(client_socket, &readerpacket, sizeof(st_request), 0) < 0) {
        perror("Send failed");
        close(client_socket);
        return;
    }

    printf("Request sent to server: %s\n", path);

    st_request req;
    while (1) {
        // Clear the structure
        memset(&req, 0, sizeof(st_request));

        // Receive data
        int bytes_received = recv(client_socket, &req, sizeof(st_request), 0);
        if (bytes_received == -1) {
            perror("Receiving data failed");
            break;
        } else if (bytes_received == 0) {
            // Connection closed by server
            printf("Connection closed by server\n");
            break;
        }

        // Debugging: Check received bytes and content
        printf("Bytes received: %d\n", bytes_received);
        printf("Data: '%.*s'\n", (int)bytes_received, req.data);

        // Ensure null-termination for safety
        if (bytes_received > 0 && bytes_received < sizeof(req.data)) {
            req.data[bytes_received] = '\0';
        }

        // Print received data
        
        printf("Received data: %s\n", req.data);

        // Handle specific request type
        if (req.request_type == 12) {
            printf("ACK received. Stopping reception.\n");
            break;
        }
    }

    // free(req);
    close(client_socket);
}

void reading_operation(char *path)
{
    int client_socket = connect_with_ns();
    printf("%s\n", path);
    st_request readerpacket;
    readerpacket.request_type = READ_REQ;
    strcpy(readerpacket.src_path, path);
    // printf("%s\n",readerpacket.data);
    // return ;
    int bytes_sent = send(client_socket, &readerpacket, sizeof(st_request), 0);
    if (bytes_sent < 0)
    {
        perror("Send failed");
    }
    st_response *response = (st_response *)malloc(sizeof(st_response));
    int bytes_received = recv(client_socket, response, sizeof(st_response), 0);
    int port;
    char *ipaddress = (char *)malloc(16 * sizeof(char));
    if (bytes_received == -1)
    {
        perror("Receive failed");
    }
    else
    {
        if (response->response_type == FILE_NOT_FOUND)
        {
            printf(RED_COLOR "File Not Found \n"RESET_COLOR) ; // Error Not Found File
            // return;
        }
        // else if (response->response_type == TIMEOUT)
        // {
        //     printf(RED_COLOR "File currently unavailable please try again\n" RESET_COLOR);
        //     // return;
        // }
        else if (response->response_type == INVALID_FILETYPE)
        {
            printf(RED_COLOR "Invalid file type\n" RESET_COLOR);
            // return;
        }
        // else if (response->response_type == SERVER_NOT_FOUND)
        // {
        //     printf(RED_COLOR "%s\n"RESET_COLOR, response->message);
        //     // close(client_socket);
        // }
        else
        {
        
            strcpy(ipaddress, response->IP_Addr);
            port = response->Port_No;
            printf("%s %d\n", ipaddress, port);
        }
    }
    close(client_socket);
    // return;
    // char *token;
    // token=strtok(response->message," ");

    if (response->response_type == 200 )
    {
        // strcpy(ipaddress, response->IP_Addr);
        // port = response->Port_No;
        // strcpy(path,response->message);
        communicate_with_ss(ipaddress, port, path);
    }
    // else
    // {
    //     communicate_with_ss_backup(ipaddress, port, path);
    // }

    return;
}
