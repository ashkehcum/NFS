#include "headers.h"
extern node* hashtable; 

// only arrive here if the request is a READ, WRITE, GET_FILE_INFO or STREAM_AUDIO
void handle_file_request(request req, int client_id){
    if(req->request_type == GET_FILE_INFO || req->request_type == READ_FILE || req->request_type == WRITE_FILE){
        if(strstr(req->path, ".txt") == NULL){
            // printf("Here\n");
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            add_to_cache(req, r);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE,0);
            return;
        }
    }
    if(req->request_type == STREAM_AUDIO) {
        if(strstr(req->path, ".mp3") == NULL){
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            add_to_cache(req, r);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE,0);
            return;
        }
    } 
    response r = (response)malloc(sizeof(st_response));
    printf("%s\n", req->path);
    int id = Get(req->path);
    printf("id: %d\n", id);
    if(id!=-1) {
        r->response_type = FILE_FOUND;
        snprintf(r->message, sizeof(r->message), "%s | %d", storage_server_list[id]->IP_Addr, storage_server_list[id]->Port_No);
        strcpy(r->IP_Addr, storage_server_list[id]->IP_Addr);
        r->Port_No = storage_server_list[id]->Client_Port;
        send(client_id, r, sizeof(st_response), 0);
        add_to_cache(req, r);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_FOUND,0);
    } else {
        r->response_type = FILE_NOT_FOUND;
        strcpy(r->message, "File not found");
        strcpy(r->IP_Addr, "");
        r->Port_No = -1;
        send(client_id, r, sizeof(st_response), 0);
        add_to_cache(req, r);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_NOT_FOUND,0);
    }
    free(r);
}


// only arrive here if the request is a COPY, CREATE or DELETE
void file_requests_to_storage_server(request req, int client_id)
{
    int storage_server_id = Get(req->path);
    request r = (request)malloc(sizeof(st_request));
    if(storage_server_id == -1)
    {
        response res = (response)malloc(sizeof(st_response));
        res->response_type = PATH_NOT_FOUND;
        strcpy(res->message, "File not found");
        strcpy(res->IP_Addr, "");
        res->Port_No = -1;
        send(client_id, res, sizeof(st_response), 0);
        add_to_cache(req, res);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
        return;
    }
    if(req->request_type == CREATE_FILE)
    {
        // send a request to the storage server
        r->request_type = CREATE_FILE;
        strcpy(r->file_or_dir_name, req->file_or_dir_name);
        strcpy(r->path, req->path);
        strcpy(r->data, req->data);  
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss respose left
    }
    else if(req->request_type == DELETE_FILE)
    {
        r->request_type = DELETE_FILE;
        strcpy(r->file_or_dir_name, req->file_or_dir_name);
        strcpy(r->path, req->path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss respose left
    }
    else
    {
        int res = Get(req->copy_to_path);
        if(res == -1)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = COPY_TO_PATH_INVALID;
            strcpy(res->message, "Path not found");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            add_to_cache(req, res);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_TO_PATH_INVALID,0);
            return;
        }
        r->request_type = COPY_FILE;
        strcpy(r->file_or_dir_name, req->file_or_dir_name);
        strcpy(r->path, req->path);
        strcpy(r->data, req->data);
        strcpy(r->copy_to_path, req->copy_to_path);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
    }
    free(r);
}