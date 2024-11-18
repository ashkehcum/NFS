#include "headers.h"
extern node* hashtable; 

// only arrive here if the request is a READ, WRITE, GET_FILE_INFO or STREAM_AUDIO
void handle_file_request(request req, int client_id){
    if(req->request_type == GET_FILE_INFO || req->request_type == READ_FILE || req->request_type == WRITE_FILE){
        if(strstr(req->src_path, ".txt") == NULL){
            // printf("Here\n");
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            add_to_cache(req, r, -1);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE,0);
            return;
        }
    }
    if(req->request_type == STREAM_AUDIO) {
        if(strstr(req->src_path, ".mp3") == NULL){
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            add_to_cache(req, r, -1);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE,0);
            return;
        }
    } 
    response r = (response)malloc(sizeof(st_response));
    printf("%s\n", req->src_path);
    int id = Get(req->src_path);
    printf("id: %d\n", id);
    int folder = 0;
    if(id!=-1) {
        // snprintf(r->message, sizeof(r->message), "%s | %d", storage_server_list[id]->IP_Addr, storage_server_list[id]->Port_No);
        if(storage_server_list[id]->is_active == false){
            if(req->request_type == WRITE_FILE || req->request_type == STREAM_AUDIO || req->request_type == GET_FILE_INFO) 
            {
                r->response_type = FILE_NOT_FOUND;
                strcpy(r->message, "Storage server is down, you can only READ the file");
                strcpy(r->IP_Addr, "");
                r->Port_No = -1;
                send(client_id, r, sizeof(st_response), 0);
                add_to_cache(req, r, -1);
                logMessage(CLIENT_FLAG, client_id, *req, FILE_NOT_FOUND,0);
                return;
            }
            else if(storage_server_list[storage_server_list[id]->backup_storage_server1]->is_active == true || storage_server_list[storage_server_list[id]->backup_storage_server2]->is_active == true){
                if(storage_server_list[storage_server_list[id]->backup_storage_server1]->is_active == true){
                    id = storage_server_list[id]->backup_storage_server1;
                    folder = storage_server_list[id]->backupfoldernumber1;
                } else {
                    id = storage_server_list[id]->backup_storage_server2;
                    folder = storage_server_list[id]->backupfoldernumber2;
                }
            } else {
                printf("Backup servers are also down\n");
                r->response_type = FILE_NOT_FOUND;
                strcpy(r->message, "File not found");
                strcpy(r->IP_Addr, "");
                r->Port_No = -1;
                send(client_id, r, sizeof(st_response), 0);
                add_to_cache(req, r, -1);
                logMessage(CLIENT_FLAG, client_id, *req, FILE_NOT_FOUND,0);
                return;
            }
        }
        r->response_type = FILE_FOUND;
        strcpy(r->IP_Addr, storage_server_list[id]->IP_Addr);
        // message(backup) | folder
        snprintf(r->message, sizeof(r->message), "Backup | %d", folder);   
        r->Port_No = storage_server_list[id]->Client_Port;
        send(client_id, r, sizeof(st_response), 0);
        add_to_cache(req, r, id);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_FOUND,0);
    } else {
        r->response_type = FILE_NOT_FOUND;
        strcpy(r->message, "File not found");
        strcpy(r->IP_Addr, "");
        r->Port_No = -1;
        send(client_id, r, sizeof(st_response), 0);
        add_to_cache(req, r, -1);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_NOT_FOUND,0);
    }
    free(r);
}


// only arrive here if the request is a COPY, CREATE or DELETE
void file_requests_to_storage_server(request req, int client_id)
{
    int storage_server_id = -1;
    if((req->request_type == DELETE_FILE || req->request_type == DELETE_DIR) && strcmp(req->src_path, "/") == 0)
    {
        storage_server_id = Get(req->src_file_dir_name);
        printf("Deleting root directory or file\n");
    }
    else
        storage_server_id = Get(req->src_path);

    request r = (request)malloc(sizeof(st_request));  // send it to the main source storage server
    if(storage_server_id == -1)
    {
        response res = (response)malloc(sizeof(st_response));
        res->response_type = PATH_NOT_FOUND;
        strcpy(res->message, "File not found");
        strcpy(res->IP_Addr, "");
        res->Port_No = -1;
        send(client_id, res, sizeof(st_response), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
        free(r);
        return;
    }
    else if(req->request_type == CREATE_FILE)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot create file");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            free(r);
            return;
        }
        // send a request to the storage server
        response res = (response)malloc(sizeof(st_response));
        r->request_type = CREATE_FILE;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data, req->data);  

        printf("Creating file\n");
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);

        // receive the response from the storage server and send it to the client
        int r1 = recv(socket_arr[storage_server_id][0], res, sizeof(st_response), 0);
        if(res->response_type == ACK)
        {
            printf("File %s created successfully\n", req->src_file_dir_name);
            // insert the path into the hash table 
            char s[100];
            snprintf(s, sizeof(s), "%s/%s", req->src_path, req->src_file_dir_name);
            printf("Inserted path: %s in the hash table", s);
            if(Get(s) == -1)
                Insert(s, storage_server_id);
            send(client_id, res, sizeof(st_response), 0);
        }
        else
        {
            response res1 = (response)malloc(sizeof(st_response));
            res1->response_type = FILE_CREATE_ERROR;
            strcpy(res1->message, "File already exists");
            strcpy(res1->IP_Addr, "");
            res1->Port_No = -1;
            send(client_id, res1, sizeof(st_response), 0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss respose left
    }
    else if(req->request_type == CREATE_DIR)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot create directory");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            free(r);
            return;
        }
        printf("Creating directory\n");
        response res = (response)malloc(sizeof(st_response));
        r->request_type = CREATE_DIR;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);

        // receive the response from the storage server and send it to the client
        int r1 = recv(socket_arr[storage_server_id][0], res, sizeof(st_response), 0);
        printf("Response received %d\n", res->response_type);
        if(res->response_type == ACK)
        {
            char s[100];
            snprintf(s, sizeof(s), "%s/%s", req->src_path, req->src_file_dir_name);
            printf("Directory %s created successfully\n", s);
            if(Get(s) == -1)
                Insert(s, storage_server_id);
            printf("Inserted path: %s in the hash table\n", s);
            send(client_id, res, sizeof(st_response), 0);
        }
        else
        {
            response res1 = (response)malloc(sizeof(st_response));
            res1->response_type = CREATE_DIR_ERROR;
            strcpy(res1->message, "Directory already exists");
            strcpy(res1->IP_Addr, "");
            res1->Port_No = -1;
            send(client_id, res1, sizeof(st_response), 0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss response left
    }
    else if(req->request_type == DELETE_FILE)
    {
        int storage_server_id1 = -1;
        char s1[100];
        // concatenate the path with the file name
        if(strcmp(req->src_path, "/") == 0)
        {
            storage_server_id1 = Get(req->src_file_dir_name);
            strcpy(s1, req->src_file_dir_name);
        }
        else
        {    
            snprintf(s1, sizeof(s1), "%s/%s", req->src_path, req->src_file_dir_name);
            storage_server_id1 = Get(s1);
        }
        printf("%s\n", s1);
        if(storage_server_list[storage_server_id1]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot delete file");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            free(r);
            return;
        }
        printf("Deleting file\n");
        response res = (response)malloc(sizeof(st_response));
        r->request_type = DELETE_FILE;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id1][0], r, sizeof(st_request), 0);

        // receive the response from the storage server and send it to the client
        int r1 = recv(socket_arr[storage_server_id1][0], res, sizeof(st_response), 0);
        if(res->response_type == ACK)
        {
            printf("File %s deleted successfully\n", s1);
            // delete the path from the hash table
            Delete(s1);
            printf("Deleted path: %s from the hash table", s1);
            send(client_id, res, sizeof(st_response), 0);
        }
        else
        {
            response res1 = (response)malloc(sizeof(st_response));
            res1->response_type = FILE_DELETE_ERROR;
            strcpy(res1->message, "File not found");
            strcpy(res1->IP_Addr, "");
            res1->Port_No = -1;
            send(client_id, res1, sizeof(st_response), 0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss response left
    }
    else if(req->request_type == DELETE_DIR)
    {
        char s1[100];
        int storage_server_id1 = -1;
        if(strcmp(req->src_path, "/") == 0)
        {
            storage_server_id1 = Get(req->src_file_dir_name);
            strcpy(s1, req->src_file_dir_name);
        }
        else
        {
            snprintf(s1, sizeof(s1), "%s/%s", req->src_path, req->src_file_dir_name);
            storage_server_id1 = Get(req->src_path);
        }
        if(storage_server_list[storage_server_id1]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot delete directory");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            free(r);
            return;
        }
        response res = (response)malloc(sizeof(st_response));
        r->request_type = DELETE_DIR;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id1][0], r, sizeof(st_request), 0);

        // receive the response from the storage server and send it to the client
        int r1 = recv(socket_arr[storage_server_id1][0], res, sizeof(st_response), 0);
        if(req->request_type == ACK)
        {
            printf("Directory %s deleted successfully\n", s1);
            // delete the path from the hash table
            Delete(s1);
            printf("Deleted path: %s from the hash table", s1);
            send(client_id, res, sizeof(st_response), 0);
        }
        else
        {
            response res1 = (response)malloc(sizeof(st_response));
            res1->response_type = DELETE_DIR_ERROR;
            strcpy(res1->message, "Directory not found");
            strcpy(res1->IP_Addr, "");
            res1->Port_No = -1;
            send(client_id, res1, sizeof(st_response), 0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss response left
    }
    else if(req->request_type == COPY_FILE)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot copy file");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            free(r);
            return;
        }
        int res4 = Get(req->dest_path);
        request des_r = (request)malloc(sizeof(st_request));
        if(res4 == -1)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = COPY_TO_PATH_INVALID;
            strcpy(res->message, "Path not found");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_TO_PATH_INVALID,0);
            free(r);
            return;
        } else if(res4 == storage_server_id){
            r = (request)malloc(sizeof(st_request));
            r->request_type = COPY_TO_SAME_FILE;
            strcpy(r->src_file_dir_name, req->src_file_dir_name);

            char full_path[100];
            snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
            char full_dest_path[100];
            snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);

            strcpy(r->src_path, full_path);
            strcpy(r->dest_path, full_dest_path);
            strcpy(r->data, req->data);     
            strcpy(r->ip_for_copy, storage_server_list[res4]->IP_Addr);
            r->port_for_copy = storage_server_list[res4]->Port_No; 
            send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
            free(r);
            return;
        }
        r->request_type = COPY_FILE;
        char copy_path[100];
        char full_path[100];
        strcpy(copy_path, req->src_path);
        snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
        char full_dest_path[100];
        snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);
        
        // tokenise the copy to get the file name
        
        // get the last token and copy it to the src_file_dir_name

        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, full_path);
        strcpy(r->dest_path, full_dest_path);
        strcpy(r->data, req->data);
        strcpy(r->ip_for_copy, storage_server_list[res4]->IP_Addr);
        r->port_for_copy = storage_server_list[res4]->Port_No;
        // printf("r->port_for_copy: %d\n", r->port_for_copy);
        r->socket = storage_server_list[res4]->ss_socket;
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        
        des_r->request_type = RECIEVE_FILE;

        full_path[100];
        snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
        full_dest_path[100];
        snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);

        char *token = strtok(copy_path, "/");
        while(token != NULL)
        {
            strcpy(des_r->src_file_dir_name, token);
            token = strtok(NULL, "/");
        }

        printf("file name %s\n", des_r->src_file_dir_name);
        strcpy(des_r->src_path, full_path);
        strcpy(des_r->dest_path, full_dest_path);
        strcpy(des_r->data, req->data);
        strcpy(des_r->ip_for_copy, storage_server_list[storage_server_id]->IP_Addr);
        des_r->port_for_copy = storage_server_list[storage_server_id]->Port_No;
        r->socket = storage_server_list[storage_server_id]->ss_socket;
        printf("Path sent : %s\n", full_dest_path);
        // printf("%d\n", storage_server_list[res]->Port_No);
        send(socket_arr[res4][0], des_r, sizeof(st_request), 0);

        response r3 = (response)malloc(sizeof(st_response));
        int r1 = recv(socket_arr[storage_server_id][0], r3, sizeof(st_response), 0);
        if(r1 < 0)
        {
            printf("Error in copying file\n");
            response res = (response)malloc(sizeof(st_response));
            res->response_type = FILE_COPY_ERROR;
            strcpy(res->message, "Copy failed");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, FILE_COPY_ERROR,0);
            return;
        }
        if(r3->response_type == ACK)
        {
            // insert the path into the hash table
            char s[100];
            snprintf(s, sizeof(s), "%s/%s", req->dest_path, req->src_file_dir_name);
            printf("Inserted path: %s in the hash table", s);
            if(Get(s) == -1)
                Insert(s, res4);
            send(client_id, r3, sizeof(st_response), 0);
        }
        else
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = FILE_COPY_ERROR;
            strcpy(res->message, "Copy failed");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, FILE_COPY_ERROR,0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
    }
    else if(req->request_type == COPY_DIR)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot copy directory");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        int res5 = Get(req->dest_path);
        request des_r = (request)malloc(sizeof(st_request));
        printf("Copying directory\n");
        printf("res5: %d\n", res5);
        printf("storage_server_id: %d\n", storage_server_id);
        if(res5 == -1)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = COPY_TO_PATH_INVALID;
            strcpy(res->message, "Path not found");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_TO_PATH_INVALID,0);
            return;
        } else if(res5 == storage_server_id){
            r = (request)malloc(sizeof(st_request));
            r->request_type = COPY_TO_SAME_DIR;
            char full_path[100];
            snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
            char full_dest_path[100];
            snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);

            strcpy(r->src_file_dir_name, req->src_file_dir_name);
            strcpy(r->src_path, full_path);
            strcpy(r->dest_path, full_dest_path);  
            strcpy(r->ip_for_copy, storage_server_list[res5]->IP_Addr);
            r->port_for_copy = storage_server_list[res5]->Port_No;
            strcpy(r->data, req->data);    
            send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
            free(r);
            return;
        }
        
        r->request_type = COPY_DIR;
        // strcpy(r->src_path, req->src_path);
        // strcpy(r->dest_path, req->dest_path);
        char full_path[100];
        char full_dest_path[100];

        char copy_path[100];
        // tokenise the copy to get the directory name
        strcpy(copy_path, req->src_path);
        char *token = strtok(copy_path, "/");
        while(token != NULL)
        {
            strcpy(des_r->src_file_dir_name, token);
            token = strtok(NULL, "/");
        }

        snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
        snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);

        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->data, req->data);
        strcpy(r->src_path, full_path);
        strcpy(r->dest_path, full_dest_path);
        strcpy(r->ip_for_copy, storage_server_list[res5]->IP_Addr);
        r->port_for_copy = storage_server_list[res5]->Port_No;
        r->socket = storage_server_list[res5]->ss_socket;

        printf("Sending request to source storage directory\n");
        printf("%d\n", storage_server_id);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);

        // sleep(1);
        // request for the destination storage server
        des_r->request_type = RECIEVE_DIR;
        full_path[100];
        full_dest_path[100];
        snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
        snprintf(full_dest_path, sizeof(full_dest_path), "main/%s", req->dest_path);

        strcpy(des_r->data, req->data);
        strcpy(des_r->src_path, full_path);
        strcpy(des_r->dest_path, full_dest_path);
        strcpy(des_r->ip_for_copy, storage_server_list[storage_server_id]->IP_Addr);
        des_r->port_for_copy = storage_server_list[storage_server_id]->Port_No;
        r->socket = storage_server_list[res5]->ss_socket;

        printf("Sending request to reciever storage directory\n");
        printf("%d\n", res5);
        send(socket_arr[res5][0], des_r, sizeof(st_request), 0);
        
        response resp1 = (response)malloc(sizeof(st_response));
        int r1 = recv(socket_arr[storage_server_id][0], resp1, sizeof(st_response), 0);
        
        if(resp1->response_type == ACK)
        {
            // insert the path into the hash table
            char s[100];
            snprintf(s, sizeof(s), "%s/%s", req->dest_path, des_r->src_file_dir_name);
            printf("Inserted path: %s in the hash table", s);
            if(Get(s) == -1)
                Insert(s, res5);
            send(client_id, resp1, sizeof(st_response), 0);
        }
        else
        {
            response res1 = (response)malloc(sizeof(st_response));
            res1->response_type = COPY_DIR_ERROR;
            strcpy(res1->message, "Copy failed");
            strcpy(res1->IP_Addr, "");
            res1->Port_No = -1;
            send(client_id, res1, sizeof(st_response), 0);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_DIR_ERROR,0);
        }
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
    }
    else
    {
        response res = (response)malloc(sizeof(st_response));
        res->response_type = INVALID_REQUEST;
        strcpy(res->message, "Invalid request");
        strcpy(res->IP_Addr, "");
        res->Port_No = -1;
        send(client_id, res, sizeof(st_response), 0);
        add_to_cache(req, res, -1);
        logMessage(CLIENT_FLAG, client_id, *req, INVALID_REQUEST,0);
    }
    free(r);
}