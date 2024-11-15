#include"headers.h"

ss storage_server_list[100];
extern node* hashtable;
int socket_arr[MAX_CONNECTIONS][2];
int storage_server_count = 0;

void* handle_client_process(void *arg) {
    // handle the client request
    proc n = (proc)arg;
    int client_id = n->client_id;
    request req = (request)malloc(sizeof(st_request));
    req->request_type = n->request_type;
    strcpy(req->data, n->data);
    strcpy(req->path, n->path);
    strcpy(req->copy_to_path, n->copy_to_path);
    strcpy(req->file_or_dir_name, n->file_or_dir_name);
    req->socket = n->socket;
    if(req->request_type == READ_FILE || req->request_type == WRITE_FILE || req->request_type == GET_FILE_INFO || req->request_type == STREAM_AUDIO) {
        handle_file_request(req, client_id);
    }
    else if(req->request_type == CREATE_FILE || req->request_type == DELETE_FILE || req->request_type == COPY_FILE) {
        file_requests_to_storage_server(req, client_id);
    }
    else if(req->request_type == LIST_PATHS)
    {   
        response r;
        printf("Printing all paths\n");
        r = Print_all_paths();
        printf("%s\n", r->message);
        send(client_id, r, sizeof(st_response), 0);
        logMessage(CLIENT_FLAG, client_id, *req, r->response_type,0);
    }
    free(req);
}

void* client_handler(void *arg)
{
    int client_socket = *(int *)arg;
    while(1)
    {
        request req = (request)malloc(sizeof(st_request));
        bzero(req, sizeof(st_request));
        int res = recv(client_socket, req, sizeof(st_request), 0);
        if(res < 0){
            free(req);
            // perror("Error in receiving request from client");
            printf("error recieving");
            break;
            // exit(1);
        } else if(res == 0){
            free(req);
            printf("Client disconnected\n");
            socket_arr[client_socket][0] = -1;
            close(client_socket);
            break;
        }
        int req_valid = 1;
        if(req->request_type == -1)
        {
            free(req);
            req->request_type = 0;
            break;
        }
        if(req_valid)        
        {
            pthread_t process;
            proc n = (proc)malloc(sizeof(req_process));
            n->client_id = client_socket;
            n->request_type = req->request_type;
            strcpy(n->data, req->data);
            strcpy(n->path, req->path);
            strcpy(n->copy_to_path, req->copy_to_path);
            strcpy(n->file_or_dir_name, req->file_or_dir_name);
            n->socket = client_socket;

            response res = get_from_cache(req);
            if(res != NULL){
                // printf("%s\n", res->message);
                // printf("%s\n", res->IP_Addr);
                // printf("%d\n", res->Port_No);
                send(client_socket, res, sizeof(st_response), 0);
                logMessage(CLIENT_FLAG, client_socket, *req, res->response_type,1);
                free(req);
                continue;
            }
            pthread_create(&process, NULL, &handle_client_process, (void *)n);
            // join the thread &process
            pthread_join(process, NULL);
        } else{
            // log the invalid request
            logMessage(CLIENT_FLAG, client_socket, *req, INVALID_REQUEST,0);
        }
        free(req);
    }
}

void* storage_server_handler()
{
    
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
                    printf("Client connected\n");
                    // create a new thread to handle the client request
                    pthread_t client_thread;
                    pthread_create(&client_thread, NULL, &client_handler, (void *)&socket_arr[i][0]);
                    // pthread_join(client_thread, NULL);
                }

                else if(strcmp(buffer, "storage_server") == 0){
                    // send a message to the storage server to confirm the connection
                    send(socket_arr[i][0], "Hello storage server", strlen("Hello storage_server"), 0);
                    socket_arr[i][1] = STORAGE_FLAG;

                    // recieve the storage server's IP address and port number
                    ss_info s = (ss_info)malloc(sizeof(storage_server_info));
                    ssize_t bytes_received = recv(socket_arr[i][0], s, sizeof(storage_server_info), 0);
                    if (bytes_received <= 0) {
                        perror("Receiving storage server info failed");
                        free(s);
                        close(socket_arr[i][0]);
                        continue;
                    }
                    //  else if (bytes_received < sizeof(storage_server_info)) {
                    //     fprintf(stderr, "Incomplete data received\n");
                    //     free(s);
                    //     close(socket_arr[i][0]);
                    //     continue;
                    // }
                    // print the contents of s
                    printf("Storage server IP: %s\n", s->IP_Addr);
                    printf("Storage server Port to NS: %d\n", s->NS_Port_No);
                    printf("Storage server Port to Client: %d\n", s->Client_Port_No);
                    printf("Storage server Paths:\n");

                    char *token = strtok(s->paths, ";");
                    while (token != NULL) {
                        printf("%s\n", token);
                        printf("Hash: %lu\n", create_hash(token));
                        Insert(token, storage_server_count);
                        printf("%d\n", Get(token));
                        token = strtok(NULL, ";");
                    }
                    
                    storage_server_list[storage_server_count] = (ss)malloc(sizeof(storage_server));
                    storage_server_list[storage_server_count]->storage_server_id = storage_server_count;
                    strcpy(storage_server_list[storage_server_count]->IP_Addr, s->IP_Addr);
                    storage_server_list[storage_server_count]->Port_No = s->NS_Port_No;
                    storage_server_list[storage_server_count]->Client_Port = s->Client_Port_No;
                    
                    storage_server_count++;
                    request req = (request)malloc(sizeof(st_request));
                    req->request_type = STORAGE_SERVER_CONNECTION;
                    logMessage(STORAGE_FLAG, socket_arr[i][0], *req, STORAGE_SERVER_CONNECTED,0);
                    free(req);
                    // create a new thread to handle the requests sent to storage server
                    pthread_t storage_thread;
                    pthread_create(&storage_thread, NULL, &storage_server_handler, (void *)&socket_arr[i][0]);
                }
            }
        }
    }
}