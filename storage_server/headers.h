#ifndef _HEADERS_H_
#define _HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/wait.h>

#define GREEN_COLOR  "\033[0;32m"
#define CYAN_COLOR   "\033[0;36m"
#define ORANGE_COLOR "\e[38;2;255;85;0m"
#define RESET_COLOR  "\033[0m"
#define BLUE_COLOR   "\033[0;34m"
#define RED_COLOR    "\033[0;31m"
#define YELLOW_COLOR "\033[0;33m"

#define MAX_CLIENTS 100
#define NS_PORT 3000
#define NS_IP "0.0.0.0"
#define MAX_PATH_LEN  1024  
#define MAX_FILES 25
#define MAX_PATHS 25

typedef struct storage_server{
    char IP_Addr[15];
    int Port_No;
    int NM_Port;
    int Client_Port;
    int has_backup;
    storage_server *backup1;
    storage_server *backup2;
    // Rem

} storage_server;

typedef struct client{
    int id;
    char IP_Addr[20];
    int Port_No;
} client;

typedef struct request{
    int request_type;
    char file_name[MAX_PATH_LEN];
} request;



#endif