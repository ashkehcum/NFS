#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "headers.h"

int register_with_naming_server(storage_server *ss, char* accessible_paths);

void *naming_server_listener(void *arg);
void *client_listener(void *arg);

int get_ip_and_port(char* ip_addr, int* port);

void* work_handler(void *arg);

//Naming Server Functions
void delete_file(char * path);
void copy_file(char* path,char * cpoy_topath);
void create_file_or_directory(st_request* req);

//Client Functions
void read_file(st_request* req);
void write_file(st_request* req);
void get_file_info(st_request* req);
void stream_audio(st_request* req);

int wait_for_ack(int client_socket, int timeout_seconds);

#endif