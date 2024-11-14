#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "headers.h"

int register_with_naming_server(storage_server *ss, char (*accessible_paths)[MAX_PATH_LEN]);

void *naming_server_listener(void *arg);
void *client_listener(void *arg);

#endif