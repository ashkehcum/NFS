#include "headers.h"

void handle_file_request(request req, int client_id){
    if(req->request_type == GET_FILE_INFO){
        if(strstr(req->data, ".txt") == NULL){
            request r = (request)malloc(sizeof(st_request));
            r->request_type = INVALID_FILETYPE;
            strcpy(r->data, "Incompatible file opened!");
            send(client_id, r, sizeof(st_request), 0);
            return;
        }
        int file_found = 0;
        int id = 0;
        response r = (response)malloc(sizeof(st_response));
        // search for the file in the storage servers
        for(int i = 0; i < storage_server_count; i++)
        {
            // if(search(ss_list[i]->root, req->data) >= 0)
            // {
            //     file_found = 1;
            //     id = i;
            //     break;
            // }
        }
    } else if(req->request_type == READ_FILE) {
        // read the file from the storage server
        // send the file to the client
    } else if(req->request_type == WRITE_FILE) {
        // write the file to the storage server
        // send the response to the client
    } else if(req->request_type == GET_FILE_INFO) {
        // get the file info from the storage server
        // send the file info to the client
    } else if(req->request_type == STREAM_AUDIO) {
        // stream the audio file from the storage server
        // send the audio file to the client
    } else{
        // invalid request
    }    
}