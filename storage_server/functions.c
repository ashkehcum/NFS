#include "functions.h"

int register_with_naming_server(storage_server *ss, char *accessible_paths)
{
    printf("ss->IP_Addr: %s\n", ss->IP_Addr);
    printf("ss->Port_No: %d\n", ss->Port_No);
    printf("ss->nm_ip: %s\n", ss->nm_ip);
    printf("ss->NM_Port: %d\n", ss->NM_Port);
    printf("accessible_paths: %s\n", accessible_paths);
    int ns_sock;
    struct sockaddr_in ns_addr;

    // Set up socket to connect to Naming Server
    ns_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ns_sock < 0)
    {
        perror("NS socket creation failed");
        return -1;
    }

    ns_addr.sin_family = AF_INET;
    ns_addr.sin_port = htons(ss->NM_Port);
    if (inet_pton(AF_INET, ss->nm_ip, &ns_addr.sin_addr) < 0)
    {
        perror("Invalid Naming Server IP address");
        close(ns_sock);
        return -1;
    }

    // Connect to Naming Server
    if (connect(ns_sock, (struct sockaddr *)&ns_addr, sizeof(ns_addr)) < 0)
    {
        perror("Failed to connect to Naming Server");
        close(ns_sock);
        return -1;
    }

    char str[] = "storage_server";
    // Send a message to the Naming Server to confirm the connection
    if (send(ns_sock, &str, sizeof(str), 0) < 0)
    {
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
    // ss_info.Port_No = ss->Port_No;
    ss_info.NS_Port_No = NS_LISTEN_PORT;
    ss_info.Client_Port_No = CLIENT_LISTEN_PORT;
    // strncpy(ss_info.paths, accessible_paths, sizeof(ss_info.paths) - 1);
    // ss_info.paths[sizeof(ss_info.paths) - 1] = '\0'; // Ensure null termination
    strcpy(ss_info.paths, accessible_paths);
    printf("ss_info.paths: %s\n", ss_info.paths);

    // Send storage_server_info to Naming Server
    if (send(ns_sock, &ss_info, sizeof(ss_info), 0) < 0)
    {
        perror("Failed to send Storage Server details to NS");
        close(ns_sock);
        return -1;
    }

    printf(GREEN_COLOR "Storage Server registered with Naming Server at %s:%d\n" RESET_COLOR, NS_IP, NS_PORT);

    // Close the socket after sending
    close(ns_sock);
    return 0;
}

void *naming_server_listener(void *arg)
{
    int ns_sock, server_sock;
    struct sockaddr_in server_addr;
    // char buffer[BUFFER_SIZE];
    int port = NS_LISTEN_PORT; // Retrieve the port from the argument

    // Create socket for listening to Naming Server
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Failed to create Naming Server listener socket");
        pthread_exit(NULL);
    }
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Naming Server listener socket bind failed");
        close(server_sock);
        pthread_exit(NULL);
    }

    printf("Listening for Naming Server requests on port %d...\n", port);

    // ns_sock = accept(server_sock, NULL, NULL);
    // if(ns_sock < 0) {
    //     perror("Failed to accept Naming Server connection");
    //     close(server_sock);
    //     pthread_exit(NULL);
    // }
    // printf("Naming Server connected.\n");
}

#define BUFFER_SIZE 1024
void *client_listener(void *arg)
{
    int threads_no = 0;
    int server_sock, client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    // int port = ((int)arg);  // Retrieve the port from the argument
    int port = CLIENT_LISTEN_PORT;

    // Create socket for listening to clients
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("Failed to create client listener socket");
        pthread_exit(NULL);
    }
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    printf("port from here:%d\n", port);

    // Bind the socket to the specified port
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Client listener socket bind failed");
        close(server_sock);
        pthread_exit(NULL);
    }
    printf("Listening for client connections on port %d...\n", port);

    while (listen(server_sock, MAX_CLIENTS) >= 0)
    {
        // Accept an incoming client connection
        client_sock = accept(server_sock, NULL, NULL);
        if (client_sock < 0)
        {
            perror("Failed to accept client connection");
            continue;
        }

        printf("Client connected.\n");

        // Handle client requests

        // receive a st_request from the client
        st_request req;
        int bytes;
        // bytes = recv(client_sock, &req, sizeof(req), 0);
        // // while ((bytes = recv(client_sock, &req, sizeof(req), 0)) > 0)
        // // {
        //     printf("%d\n", bytes);
        //     // if (bytes == sizeof(st_request))
        //     // {
        //     //     break;
        //     // }
        // // }
        int bytes_received = recv(client_sock, &req, sizeof(req), 0);
        if (bytes_received < 0) {
            perror("Failed to receive st_request from client");
        } else if (bytes_received == 0) {
            printf("Client disconnected\n");
        } else {
            // printf("Request received from client\n");
            // printf("Request type: %d\n", req.request_type);
            // printf("Data: %s\n", req.data);
            // printf("Path: %s\n", req.path);
            // printf("File or dir name: %s\n", req.file_or_dir_name);
            // printf("Copy to path: %s\n", req.copy_to_path);
            // printf("Socket: %d\n", req.socket);
        }

        // printf("Request received from client\n");
        // printf("Request type: %d\n", req.request_type);
        // printf("Data: %s\n", req.data);
        // printf("Path: %s\n", req.path);
        // printf("File or dir name: %s\n", req.file_or_dir_name);
        // printf("Copy to path: %s\n", req.copy_to_path);
        // printf("socket: %d\n", req.socket);
        pthread_t client_req;
        req.socket = client_sock;
        // pthread_create(&client_req, NULL, work_handler, (void *)&req);
        if (threads_no < MAX_THREADS)
        {
            pthread_create(&client_req, NULL, work_handler, (void *)&req);
            threads_no++;
        }
        else
        {
            printf("Max threads reached\n");
        }
        pthread_join(client_req, NULL);

        // Close client socket after the interaction is done
        close(client_sock);
    }

    // Close server socket when done (will not be reached in this example)
    close(server_sock);
    pthread_exit(NULL);
}

void *work_handler(void *arg)
{
    st_request req = *((st_request *)arg);
    // printf("Request type: %d\n", req.request_type);
    // printf("Data: %s\n", req.data);
    // printf("Path: %s\n", req.path);
    // printf("File or dir name: %s\n", req.file_or_dir_name);
    // printf("Copy to path: %s\n", req.copy_to_path);
    switch (req.request_type)
    {
    case READ_FILE:
        printf("Reading file at handker \n");
        read_file(&req);
        break;
    case WRITE_FILE:
        write_file(&req);
        break;
    case GET_FILE_INFO:
        get_file_info(&req);
        break;
    case STREAM_AUDIO:
        printf("Streaming audio at handler\n");
        printf("path at handler: %s\n", req.path);
        stream_audio(&req);
        break;
    case COPY_FILE:
        break;
    case DELETE_FILE:
        break;
    case CREATE_FILE:
        create_file_or_directory(&req);
        break;
    case COPY_DIR:
        break;
    case DELETE_DIR:
        break;
    case CREATE_DIR:
        create_file_or_directory(&req);
        break;
    case FILE_FOUND:
        break;
    default:
        break;
    }
}

int wait_for_ack(int client_socket, int timeout_seconds)
{
    char ack_buffer[16];
    struct timeval timeout;
    fd_set read_fds;

    // Initialize timeout structure
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;

    // Set up the file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(client_socket, &read_fds);

    // Wait for data to be available on the socket
    int retval = select(client_socket + 1, &read_fds, NULL, NULL, &timeout);
    if (retval == -1)
    {
        perror("Error during select for ACK");
        return -1; // Error
    }
    else if (retval == 0)
    {
        printf("ACK not received: Timeout\n");
        return 0; // Timeout
    }

    // If data is available, read it
    int bytes_received = recv(client_socket, ack_buffer, sizeof(ack_buffer) - 1, 0);
    if (bytes_received <= 0)
    {
        perror("Failed to receive ACK");
        return -1; // Error
    }

    ack_buffer[bytes_received] = '\0'; // Null-terminate the string
    if (strcmp(ack_buffer, "ACK") == 0)
    {
        return 1; // ACK received
    }

    printf("Unexpected response instead of ACK: %s\n", ack_buffer);
    return -1; // Error
}