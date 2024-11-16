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
    int storage_server_id = Get(req->src_path);
    request r = (request)malloc(sizeof(st_request));  // send it to the main source storage server
    if(storage_server_id == -1)
    {
        response res = (response)malloc(sizeof(st_response));
        res->response_type = PATH_NOT_FOUND;
        strcpy(res->message, "File not found");
        strcpy(res->IP_Addr, "");
        res->Port_No = -1;
        send(client_id, res, sizeof(st_response), 0);
        add_to_cache(req, res, -1);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
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
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        // send a request to the storage server
        r->request_type = CREATE_FILE;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data, req->data);  
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
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
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        r->request_type = CREATE_DIR;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss response left
    }
    else if(req->request_type == DELETE_FILE)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot delete file");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        r->request_type = DELETE_FILE;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
        logMessage(CLIENT_FLAG, client_id, *req, PATH_FOUND,0);
        // ss response left
    }
    else if(req->request_type == DELETE_DIR)
    {
        if(storage_server_list[storage_server_id]->is_active == false)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = PATH_NOT_FOUND;
            strcpy(res->message, "Storage server is down, cannot delete directory");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        r->request_type = DELETE_DIR;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data,req->data);
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);
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
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        int res = Get(req->dest_path);
        request des_r = (request)malloc(sizeof(st_request));
        if(res == -1)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = COPY_TO_PATH_INVALID;
            strcpy(res->message, "Path not found");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_TO_PATH_INVALID,0);
            return;
        }
        r->request_type = COPY_FILE;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data, req->data);
        strcpy(r->dest_path, req->dest_path);
        strcpy(r->ip_for_copy, storage_server_list[res]->IP_Addr);
        r->port_for_copy = storage_server_list[res]->Port_No;
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);

        // request for the destination storage server
        des_r->request_type = RECIEVE_FILE;
        strcpy(des_r->src_file_dir_name, req->src_file_dir_name);
        strcpy(des_r->src_path, req->src_path);
        strcpy(des_r->data, req->data);
        strcpy(des_r->dest_path, req->dest_path);
        strcpy(des_r->ip_for_copy, storage_server_list[storage_server_id]->IP_Addr);
        des_r->port_for_copy = storage_server_list[storage_server_id]->Port_No;
        send(socket_arr[res][0], des_r, sizeof(st_request), 0);
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
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, PATH_NOT_FOUND,0);
            return;
        }
        int res = Get(req->dest_path);
        request des_r = (request)malloc(sizeof(st_request));
        if(res == -1)
        {
            response res = (response)malloc(sizeof(st_response));
            res->response_type = COPY_TO_PATH_INVALID;
            strcpy(res->message, "Path not found");
            strcpy(res->IP_Addr, "");
            res->Port_No = -1;
            send(client_id, res, sizeof(st_response), 0);
            add_to_cache(req, res, -1);
            logMessage(CLIENT_FLAG, client_id, *req, COPY_TO_PATH_INVALID,0);
            return;
        }
        r->request_type = COPY_DIR;
        strcpy(r->src_file_dir_name, req->src_file_dir_name);
        strcpy(r->src_path, req->src_path);
        strcpy(r->data, req->data);
        strcpy(r->dest_path, req->dest_path);
        strcpy(r->ip_for_copy, storage_server_list[res]->IP_Addr);
        r->port_for_copy = storage_server_list[res]->Port_No;
        send(socket_arr[storage_server_id][0], r, sizeof(st_request), 0);

        // request for the destination storage server
        des_r->request_type = RECIEVE_DIR;
        strcpy(des_r->src_file_dir_name, req->src_file_dir_name);
        strcpy(des_r->src_path, req->src_path);
        strcpy(des_r->data, req->data);
        strcpy(des_r->dest_path, req->dest_path);
        strcpy(des_r->ip_for_copy, storage_server_list[storage_server_id]->IP_Addr);
        des_r->port_for_copy = storage_server_list[storage_server_id]->Port_No;
        send(socket_arr[res][0], des_r, sizeof(st_request), 0);
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