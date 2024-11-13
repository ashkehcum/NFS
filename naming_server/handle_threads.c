#include"headers.h"

ss storage_server_list[100];

void handle_client_process(void *arg) {
    // handle the client request
    proc n = (proc)arg;
    int client_id = n->client_id;
    request req = (request)malloc(sizeof(st_request));
    req->request_type = n->request_type;
    strcpy(req->data, n->data);
    if(req->request_type == READ_FILE || req->request_type == WRITE_FILE || req->request_type == GET_FILE_INFO || req->request_type == STREAM_AUDIO)
    {
       handle_file_request(req, client_id);
    }
}
void client_handler(void *arg)
{
    int client_socket = *(int *)arg;
    while(1)
    {
        request req = (request)malloc(sizeof(st_request));
        int res = recv(client_socket, req, sizeof(st_request), 0);
        if(res <= 0){
            free(req);
            perror("Error in receiving request from client");
            exit(1);
        }
        int req_valid = 1;
        if(req_valid)        
        {
            pthread_t process;
            proc n = (proc)malloc(sizeof(req_process));
            n->request_type = req->request_type;
            strcpy(n->data, req->data);
            n->client_id = client_socket;
            pthread_create(&process, NULL, &handle_client_process, (void *)n);
        }
    }
}

void storage_server_handler(void *arg)
{
    int storage_server_id = *(int *)arg;
    while(1)
    {
        request req = (request)malloc(sizeof(st_request));
        int res = recv(storage_server_id, req, sizeof(st_request), 0);
        if(res <= 0){
            free(req);
            perror("Error in receiving request from storage server");
            exit(1);
        }
        int req_valid = 1;
    }
}

void *work_handler(){
    // create a socket for the naming server
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("Socket creation failed");
        exit(1);
    }
    // make the port reusable
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_socket);
        exit(EXIT_FAILURE);
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
                if(strcmp(buffer, "client") == 0)
                {
                    // send a message to the client to confirm the connection 
                    send(socket_arr[i][0], "Hello client server", strlen("Hello client server"), 0);
                    socket_arr[i][1] = CLIENT_FLAG;

                    // create a new thread to handle the client request
                    pthread_t client_thread;
                    pthread_create(&client_thread, NULL, &client_handler, (void *)&socket_arr[i][0]);
                }

                else if(strcmp(buffer, "storage_server") == 0){
                    // send a message to the storage server to confirm the connection
                    send(socket_arr[i][0], "Hello storage server", strlen("Hello storage_server"), 0);
                    socket_arr[i][1] = STORAGE_FLAG;

                    // recieve the storage server's IP address and port number 
                    storage_server_info* ss_info = (storage_server_info *)malloc(sizeof(storage_server_info));
                    recv(socket_arr[i][0], ss_info, sizeof(storage_server_info), 0);

                    storage_server_list[storage_server_count] = ss_info;    
                    storage_server_count++;

                    


                    // create a new thread to handle the storage server request
                    pthread_t storage_server_thread;
                    pthread_create(&storage_server_thread, NULL, &storage_server_handler, (void *)i);
                }
            }
        }
    }
}