#include"headers.h"

void *work_handler(){
    // create a socket for the naming server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Socket creation failed");
        exit(1);
    }
    // make the port reusable
    int opt = 1;
    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1){
        perror("Setsockopt failed");
        exit(1);
    }
    // bind the socket to the naming server's IP and port
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NS_PORT);
    server_addr.sin_addr.s_addr = inet_addr(NS_IP);
    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("Binding to port failed");
        exit(1);
    }
    // listen for incoming connections
    if(listen(server_socket, MAX_CONNECTIONS) == -1){
        perror("Listening on port failed");
        exit(1);
    }
    // accept incoming connections
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // initialize the socket array
    for(int i = 0; i < MAX_CONNECTIONS; i++){
        socket_arr[i][0] = -1;
        socket_arr[i][1] = -1;
    }
    while(1){
        // handle multiple clients
        for(int i = 0; i < MAX_CONNECTIONS; i++){
            if(socket_arr[i][0] == -1){
                socket_arr[i][0] = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
                if(socket_arr[i][0] == -1){
                    perror("Accepting connection failed");
                    exit(1);
                }
                // recieve a message from the client or storage server to determine the type of connection
                char buffer[1024] = {0};
                read(socket_arr[i][0], buffer, 1024);
                if(strcmp(buffer, "client") == 0){
                    // create a new thread to handle the client request
                    pthread_t client_thread;
                    pthread_create(&client_thread, NULL, &client_handler, (void *)i);
                }
                else if(strcmp(buffer, "storage_server") == 0){
                    // create a new thread to handle the storage server request
                    pthread_t storage_server_thread;
                    pthread_create(&storage_server_thread, NULL, &storage_server_handler, (void *)i);
                }
            }
        }
    }
}