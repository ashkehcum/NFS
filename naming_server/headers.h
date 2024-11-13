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
#include <stdbool.h>

#define GREEN_COLOR  "\033[0;32m"
#define CYAN_COLOR   "\033[0;36m"
#define ORANGE_COLOR "\e[38;2;255;85;0m"
#define RESET_COLOR  "\033[0m"
#define BLUE_COLOR   "\033[0;34m"
#define RED_COLOR    "\033[0;31m"
#define YELLOW_COLOR "\033[0;33m"

#define MAX_CLIENTS 100
#define MAX_DATA_LENGTH 100000
#define NS_PORT 3000
#define MAX_CONNECTIONS 120
#define NS_IP "0.0.0.0"
#define MAX_PATH_LEN  1024  
#define MAX_REQUEST_LEN 1000
#define STORAGE_FLAG 1
#define CLIENT_FLAG 2
#define MAX_FILES 25
#define READ_FILE 1
#define WRITE_FILE 2
#define GET_FILE_INFO 3
#define STREAM_AUDIO 4
#define INVALID_FILETYPE 402
#define FILE_NOT_FOUND 404

extern int socket_arr[MAX_CONNECTIONS][2];

void *work_handler();


typedef struct st_request
{
    int request_type;
    char data[MAX_REQUEST_LEN];
} st_request;

typedef st_request* request;

typedef struct req_process
{
    int client_id;
    int request_type;
    char data[MAX_DATA_LENGTH];
}req_process;


typedef req_process* proc;


typedef struct st_response
{
    int response_type;
    char message[MAX_REQUEST_LEN];
} st_response;

typedef st_response* response;


typedef struct storage_server{
    char IP_Addr[15];
    int Port_No;
    int Client_Port;
    // Rem

} storage_server_info;

typedef storage_server_info* ss;

typedef struct client{
    char IP_Addr;
    int Port_No;
} client;

extern client client_list[MAX_CLIENTS];
extern ss storage_server_list[100];
extern int storage_server_count = 0;

typedef struct req{

} req;

void handle_file_request(request req, int client_id);

#endif