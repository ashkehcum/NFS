#include "functions.h"

int register_with_naming_server(storage_server *ss, char (*accessible_paths)[MAX_PATH_LEN]) {
    int ns_sock;
    struct sockaddr_in ns_addr;

    // Set up connection to the Naming Server
    ns_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ns_sock < 0) {
        perror("NS socket creation failed");
        return -1;
    }

    ns_addr.sin_family = AF_INET;
    ns_addr.sin_port = htons(NS_PORT);
    inet_pton(AF_INET, NS_IP, &ns_addr.sin_addr);

    // Connect to Naming Server
    if (connect(ns_sock, (struct sockaddr *)&ns_addr, sizeof(ns_addr)) < 0) {
        perror("Failed to connect to Naming Server");
        close(ns_sock);
        return -1;
    }

    // Send Storage Server details to Naming Server
    if (send(ns_sock, ss, sizeof(*ss), 0) < 0) {
        perror("Failed to send Storage Server details to NS");
        close(ns_sock);
        return -1;
    }

    // Send accessible paths to Naming Server
    for (int i = 0; i < MAX_PATHS && accessible_paths[i][0] != '\0'; i++) {
        if (send(ns_sock, accessible_paths[i], strlen(accessible_paths[i]), 0) < 0) {
            perror("Failed to send accessible path to NS");
            close(ns_sock);
            return -1;
        }
    }

    printf(GREEN_COLOR "Storage Server registered with Naming Server at %s:%d\n" RESET_COLOR, NS_IP, NS_PORT);
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

void *client_listener(void *arg) {
    storage_server *ss = (storage_server *)arg;
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Set up a listening socket for clients
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Client socket creation failed");
        pthread_exit(NULL);
    }

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(ss->Client_Port);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Binding client socket failed");
        close(client_sock);
        pthread_exit(NULL);
    }

    listen(client_sock, 5);
    printf(YELLOW_COLOR "Listening for client requests on port %d\n" RESET_COLOR, ss->Client_Port);

    while (1) {
        int new_sock = accept(client_sock, NULL, NULL);
        if (new_sock < 0) {
            perror("Failed to accept connection from client");
            continue;
        }

        // Handle client request (e.g., read/write files)
        // Placeholder for client request handling code

        close(new_sock);
    }

    close(client_sock);
    pthread_exit(NULL);
}