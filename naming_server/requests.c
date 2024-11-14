#include "headers.h"
extern node* hashtable; 

void handle_file_request(request req, int client_id){
    if(req->request_type == GET_FILE_INFO || req->request_type == READ_FILE || req->request_type == WRITE_FILE){
        if(strstr(req->data, ".txt") == NULL){
            printf("Here\n");
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            return;
        }
    }
    if(req->request_type == STREAM_AUDIO)
    {
        if(strstr(req->data, ".mp3") == NULL){
            response r = (response)malloc(sizeof(st_response));
            r->response_type = INVALID_FILETYPE;
            strcpy(r->message, "Incompatible file opened!");
            strcpy(r->IP_Addr, "");
            r->Port_No = -1;
            send(client_id, r, sizeof(st_request), 0);
            return;
        }
    } 
    int file_found = 0;
    int id = 0;
    response r = (response)malloc(sizeof(st_response));
    // search for the file in the storage servers
    for(int i = 0; i < storage_server_count; i++)
    {
        // search all the storage servers for the file
        int value = create_hash(req->data, primes, strlen(req->data));
        if(get(hashtable, req->data, value) != -1)
        {
            file_found = 1;
            id = i;
            break;
        }  
    }
    if(file_found)
    {
        r->response_type = FILE_FOUND;
        snprintf(r->message, sizeof(r->message), "%s | %d", storage_server_list[id]->IP_Addr, storage_server_list[id]->Port_No);
        strcpy(r->IP_Addr, storage_server_list[id]->IP_Addr);
        r->Port_No = storage_server_list[id]->Port_No;
        send(client_id, r, sizeof(st_response), 0);
    }
    else
    {
        r->response_type = FILE_NOT_FOUND;
        strcpy(r->message, "File not found");
        strcpy(r->IP_Addr, "");
        r->Port_No = -1;
        send(client_id, r, sizeof(st_response), 0);
    }
}