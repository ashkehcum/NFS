#include "headers.h"
extern node* hashtable; 

void handle_file_request(request req, int client_id){
    if(req->request_type == GET_FILE_INFO || req->request_type == READ_FILE || req->request_type == WRITE_FILE){
        if(strstr(req->data, ".txt") == NULL){
            // printf("Here\n");
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE);
            return;
        }
    }
    if(req->request_type == STREAM_AUDIO) {
        if(strstr(req->data, ".mp3") == NULL){
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            logMessage(CLIENT_FLAG, client_id, *req, INVALID_FILETYPE);
            return;
        }
    } 
    response r = (response)malloc(sizeof(st_response));
    int id = Get(hashtable, req->path);
    if(id!=-1) {
        r->response_type = FILE_FOUND;
        snprintf(r->message, sizeof(r->message), "%s | %d", storage_server_list[id]->IP_Addr, storage_server_list[id]->Port_No);
        strcpy(r->IP_Addr, storage_server_list[id]->IP_Addr);
        r->Port_No = storage_server_list[id]->Client_Port;
        send(client_id, r, sizeof(st_response), 0);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_FOUND);
    } else {
        r->response_type = FILE_NOT_FOUND;
        strcpy(r->message, "File not found");
        strcpy(r->IP_Addr, "");
        r->Port_No = -1;
        send(client_id, r, sizeof(st_response), 0);
        logMessage(CLIENT_FLAG, client_id, *req, FILE_NOT_FOUND);
    }
}