#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "headers.h"

int register_with_naming_server(storage_server *ss, char* accessible_paths);

void *naming_server_listener(void *arg);
void *client_listener(void *arg);

int get_ip_and_port(char* ip_addr, int* port);

#endif