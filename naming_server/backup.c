#include"headers.h"

void backup_dir_1(int i)
{
    request s_req = (request)malloc(sizeof(st_request));
    request des_r = (request)malloc(sizeof(st_request));
    s_req->request_type = COPY_DIR;
    strcpy(s_req->src_path, "main");
    // strcpy(des_r->dest_path, "");
    if(storage_server_list[i]->backupfoldernumber1 == 1)
    {
        strcpy(des_r->dest_path, "b1");
    }
    else if(storage_server_list[i]->backupfoldernumber1 == 2)
    {
        strcpy(des_r->dest_path, "b2");
    }
    else
    {
        strcpy(des_r->dest_path, "b3");
    }
    strcpy(s_req->ip_for_copy, storage_server_list[storage_server_list[i]->backup_storage_server1]->IP_Addr);
    s_req->port_for_copy = storage_server_list[storage_server_list[i]->backup_storage_server1]->Port_No;
    strcpy(s_req->src_file_dir_name, "main");
    strcpy(des_r->src_file_dir_name, "main");
    send(storage_server_list[i]->storage_server_socket, s_req, sizeof(st_request), 0);

    des_r->request_type = RECIEVE_DIR;
    strcpy(des_r->src_path, "main");
    strcpy(des_r->dest_path, s_req->dest_path);
    strcpy(des_r->ip_for_copy, storage_server_list[i]->IP_Addr);
    // strcpy(des_r->src_file_dir_name, "main");
    des_r->port_for_copy = storage_server_list[i]->Port_No;
    send(storage_server_list[storage_server_list[i]->backup_storage_server1]->storage_server_socket, des_r, sizeof(st_request), 0);

    response res = (response)malloc(sizeof(st_response));
    int r = recv(storage_server_list[i]->storage_server_socket, res, sizeof(st_response), 0);
    if(res->response_type == ACK)
    {
        printf("Files copied successfully\n");
    }
    else
    {
        printf("Error in backing up\n");
    }
}

void backup_dir_2(int i)
{
    request s_req = (request)malloc(sizeof(st_request));
    request des_r = (request)malloc(sizeof(st_request));
    s_req->request_type = COPY_DIR;
    strcpy(s_req->src_path, "main");
    // strcpy(des_r->dest_path, ".");
    if(storage_server_list[i]->backupfoldernumber2 == 1)
    {
        strcpy(des_r->dest_path, "b1");
    }
    else if(storage_server_list[i]->backupfoldernumber2 == 2)
    {
        strcpy(des_r->dest_path, "b2");
    }
    else
    {
        strcpy(des_r->dest_path, "b3");
    }
    strcpy(s_req->ip_for_copy, storage_server_list[storage_server_list[i]->backup_storage_server2]->IP_Addr);
    s_req->port_for_copy = storage_server_list[storage_server_list[i]->backup_storage_server2]->Port_No;
    strcpy(s_req->src_file_dir_name, "main");
    strcpy(des_r->src_file_dir_name, "main");
    s_req->socket = storage_server_list[i]->storage_server_socket;
    send(storage_server_list[i]->storage_server_socket, s_req, sizeof(st_request), 0);


    des_r->request_type = RECIEVE_DIR;
    strcpy(des_r->src_path, "main");
    strcpy(des_r->dest_path, s_req->dest_path);
    strcpy(des_r->ip_for_copy, storage_server_list[i]->IP_Addr);
    // strcpy(des_r->src_file_dir_name, "main");
    des_r->port_for_copy = storage_server_list[i]->Port_No;
    des_r->socket = storage_server_list[i]->storage_server_socket;
    send(storage_server_list[storage_server_list[i]->backup_storage_server2]->storage_server_socket, des_r, sizeof(st_request), 0);
    response res = (response)malloc(sizeof(st_response));
    int r = recv(storage_server_list[i]->storage_server_socket, res, sizeof(st_response), 0);
    if(res->response_type == ACK)
    {
        printf("Files copied successfully\n");
    }
    else
    {
        printf("Error in backing up\n");
    }
}

void* backup_handler(){
    int backup_count = 0;
   while(1)
   {
        if(storage_server_count >= 3)
        {
            for(int i=0;i<storage_server_count;i++)
            {
                if(storage_server_list[i]->is_backed_up == false)
                {
                    printf("Backing up storage number: %d\n", backup_count);
                    if(i == 0)
                    {
                        storage_server_list[i]->backup_storage_server1 = 1;
                        storage_server_list[i]->backupfoldernumber1 = 1;
                        storage_server_list[i]->backup_storage_server2 = 2;
                        storage_server_list[i]->backupfoldernumber2 = 1;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    else if(i == 1)
                    {
                        storage_server_list[i]->backup_storage_server1 = 0;
                        storage_server_list[i]->backupfoldernumber1 = 1;
                        storage_server_list[i]->backup_storage_server2 = 2;
                        storage_server_list[i]->backupfoldernumber2 = 2;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    else if(i == 2)
                    {
                        storage_server_list[i]->backup_storage_server1 = 1;
                        storage_server_list[i]->backupfoldernumber1 = 2;
                        storage_server_list[i]->backup_storage_server2 = 0;
                        storage_server_list[i]->backupfoldernumber2 = 2;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    else if(i==3)
                    {
                        storage_server_list[i]->backup_storage_server1 = 0;
                        storage_server_list[i]->backupfoldernumber1 = 3;
                        storage_server_list[i]->backup_storage_server2 = 1;
                        storage_server_list[i]->backupfoldernumber2 = 3;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    else if(i==4)
                    {
                        storage_server_list[i]->backup_storage_server1 = 2;
                        storage_server_list[i]->backupfoldernumber1 = 3;
                        storage_server_list[i]->backup_storage_server2 = 3;
                        storage_server_list[i]->backupfoldernumber2 = 1;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    else
                    {
                        storage_server_list[i]->backup_storage_server1 = (i-1)%storage_server_count;
                        storage_server_list[i]->backupfoldernumber1 = 1;
                        storage_server_list[i]->backup_storage_server2 = (i-2)%storage_server_count;
                        storage_server_list[i]->backupfoldernumber2 = 2;
                        storage_server_list[i]->is_backed_up = true;
                    }
                    backup_dir_1(i); // i is the index of the storage server which is to be backed up
                    // sleep(10);
                    backup_dir_2(i);
                    backup_count++;
                    // return NULL;
                } 
            }
        }
   }
}