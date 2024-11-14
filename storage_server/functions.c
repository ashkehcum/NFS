#include "functions.h"

int register_with_naming_server(storage_server *ss, char *accessible_paths) {
    printf("ss->IP_Addr: %s\n", ss->IP_Addr);
    printf("ss->Port_No: %d\n", ss->Port_No);   
    printf("ss->nm_ip: %s\n", ss->nm_ip);
    printf("ss->NM_Port: %d\n", ss->NM_Port);
    printf("accessible_paths: %s\n", accessible_paths);
    int ns_sock;
    struct sockaddr_in ns_addr;

    // Set up socket to connect to Naming Server
    ns_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ns_sock < 0) {
        perror("NS socket creation failed");
        return -1;
    }

    ns_addr.sin_family = AF_INET;
    ns_addr.sin_port = htons(ss->NM_Port);
    if (inet_pton(AF_INET, ss->nm_ip, &ns_addr.sin_addr) < 0) {
        perror("Invalid Naming Server IP address");
        close(ns_sock);
        return -1;
    }

    // Connect to Naming Server
    if (connect(ns_sock, (struct sockaddr *)&ns_addr, sizeof(ns_addr)) < 0) {
        perror("Failed to connect to Naming Server");
        close(ns_sock);
        return -1;
    }

    char str[]="storage_server";
    // Send a message to the Naming Server to confirm the connection
    if (send(ns_sock, &str, sizeof(str), 0) < 0) {
        perror("Failed to send message to NS");
        close(ns_sock);
        return -1;
    }
    // receive message from the naming server
    char buffer[1024] = {0};
    // read(ns_sock, buffer, 1024);
    recv(ns_sock, buffer, 1024, 0);
    printf("%s\n", buffer);


    // Populate storage_server_info structure
    storage_server_info ss_info;
    // strncpy(ss_info.IP_Addr, ss->IP_Addr, sizeof(ss_info.IP_Addr) - 1);
    // ss_info.IP_Addr[sizeof(ss_info.IP_Addr) - 1] = '\0'; // Ensure null termination
    strcpy(ss_info.IP_Addr, ss->IP_Addr);
    ss_info.Port_No = ss->Port_No;
    // strncpy(ss_info.paths, accessible_paths, sizeof(ss_info.paths) - 1);
    // ss_info.paths[sizeof(ss_info.paths) - 1] = '\0'; // Ensure null termination
    strcpy(ss_info.paths, accessible_paths);
    printf("ss_info.paths: %s\n", ss_info.paths);

    // Send storage_server_info to Naming Server
    if (send(ns_sock, &ss_info, sizeof(ss_info), 0) < 0) {
        perror("Failed to send Storage Server details to NS");
        close(ns_sock);
        return -1;
    }

    printf(GREEN_COLOR "Storage Server registered with Naming Server at %s:%d\n" RESET_COLOR, NS_IP, NS_PORT);

    // Close the socket after sending
    close(ns_sock);
    return 0;
}

void *naming_server_listener(void *arg) {
    storage_server *ss = (storage_server *)arg;
    int ns_sock;
    struct sockaddr_in ns_addr;

    // Create a socket for Naming Server communication
    ns_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ns_sock < 0) {
        perror("NS socket creation failed");
        pthread_exit(NULL);
    }

    ns_addr.sin_family = AF_INET;
    ns_addr.sin_port = htons(ss->Port_No);
    ns_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind and listen for commands from the Naming Server
    if (bind(ns_sock, (struct sockaddr *)&ns_addr, sizeof(ns_addr)) < 0) {
        perror("Binding NS listener socket failed");
        close(ns_sock);
        pthread_exit(NULL);
    }

    listen(ns_sock, 5);
    printf(CYAN_COLOR "Listening for Naming Server commands on port %d\n" RESET_COLOR, ss->Port_No);

    while (1) {
        int new_sock = accept(ns_sock, NULL, NULL);
        if (new_sock < 0) {
            perror("Failed to accept connection from NS");
            continue;
        }

        // Handle Naming Server command (e.g., file operations)
        // Placeholder for handling NS commands

        close(new_sock);
    }

    close(ns_sock);
    pthread_exit(NULL);
}

#define BUFFER_SIZE 1024
void *client_listener(void *arg) {
    int server_sock, client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int port = *((int *)arg);  // Retrieve the port from the argument

    // Create socket for listening to clients
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Failed to create client listener socket");
        pthread_exit(NULL);
    }
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket to the specified port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Client listener socket bind failed");
        close(server_sock);
        pthread_exit(NULL);
    }

    // Listen for incoming connections
    // if (listen(server_sock, MAX_CLIENTS) < 0) {
    //     perror("Failed to listen on client listener socket");
    //     close(server_sock);
    //     pthread_exit(NULL);
    // }

    printf("Listening for client connections on port %d...\n", port);

    while (listen(server_sock, MAX_CLIENTS) >= 0) {
        // Accept an incoming client connection
        client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0) {
            perror("Failed to accept client connection");
            continue;
        }

        printf("Client connected.\n");

        // Handle client requests in a loop
        // while (1) {
        //     // Clear the buffer
        //     memset(buffer, 0, BUFFER_SIZE);

        //     // Receive data from the client
        //     ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        //     if (bytes_received < 0) {
        //         perror("Failed to receive data from client");
        //         break;  // Exit the loop if there's an error
        //     } else if (bytes_received == 0) {
        //         printf("Client disconnected.\n");
        //         break;  // Exit the loop if the client disconnects
        //     }

        //     buffer[bytes_received] = '\0';  // Null-terminate the received data
        //     printf("Received from client: %s\n", buffer);

        //     // Process the client's request (add your logic here)
        //     // For now, we just echo back the message
        //     const char *response = "Request received and processed";
        //     if (send(client_sock, response, strlen(response), 0) < 0) {
        //         perror("Failed to send response to client");
        //         break;
        //     }
        // }

        // receive a st_request from the client
        st_request req;
        recv(client_sock, &req, sizeof(req), 0);
        printf("Received request from client: %s\n", req.data);

        send(client_sock, "Hello client server from storage server", strlen("Hello client server from storage server"), 0);
        // st_request req2;
        // strcpy(req2.data, "Hello from storage server");
        // send(client_sock, &req2, sizeof(req2), 0);

        // Close client socket after the interaction is done
        close(client_sock);
    }

    // Close server socket when done (will not be reached in this example)
    close(server_sock);
    pthread_exit(NULL);
}