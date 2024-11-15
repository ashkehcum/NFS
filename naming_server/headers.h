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

// colors for printing
#define GREEN_COLOR  "\033[0;32m"
#define CYAN_COLOR   "\033[0;36m"
#define ORANGE_COLOR "\e[38;2;255;85;0m"
#define RESET_COLOR  "\033[0m"
#define BLUE_COLOR   "\033[0;34m"
#define RED_COLOR    "\033[0;31m"
#define YELLOW_COLOR "\033[0;33m"

// port defining
#define NS_PORT 3000
#define NS_IP "0.0.0.0"

// max limits set
#define MAX_CLIENTS 100
#define MAX_DATA_LENGTH 100000
#define MAX_CONNECTIONS 120
#define MAX_FILE_NAME 30
#define MAX_REQUEST_LEN 1000
#define MAX_FILES 25
#define MAX_PATHS 100
#define MAX_PATH_LEN 50

// flags
#define STORAGE_FLAG 0
#define CLIENT_FLAG 1

// Response types
#define FILE_FOUND 200
// Error codes
#define INVALID_REQUEST 400
#define INVALID_FILETYPE 402
#define FILE_NOT_FOUND 404
#define FILE_ALREADY_EXISTS 405
#define FILE_WRITE_ERROR 406
#define FILE_READ_ERROR 407
#define FILE_DELETE_ERROR 408
#define FILE_CREATE_ERROR 409
#define FILE_COPY_ERROR 410

// Request types
#define READ_FILE 1
#define WRITE_FILE 2
#define GET_FILE_INFO 3
#define STREAM_AUDIO 4
#define DELETE_FILE 5
#define CREATE_FILE 6
#define COPY_FILE 7
#define COPY_DIR 8
#define DELETE_DIR 9
#define CREATE_DIR 10

// Structures
typedef struct storage_server{
    int storage_server_id;
    char IP_Addr[16];
    int Port_No; // connected to the naming server
    int Client_Port; // connected to the client
    
    struct storage_server *b1;
    struct storage_server *b2;

} storage_server;
typedef storage_server* ss;

typedef struct storage_server_info {
    char IP_Addr[16];
    int Port_No;
    char paths[MAX_PATHS*MAX_PATH_LEN];
} storage_server_info;
typedef storage_server_info* ss_info;

typedef struct client {
    int client_id;
    char IP_Addr[16];
    int Port_No;
} client;

typedef struct st_request {
    int request_type;
    char data[MAX_REQUEST_LEN];
    char path[MAX_PATH_LEN];
    char copy_to_path[MAX_PATH_LEN];
    char file_or_dir_name[MAX_FILE_NAME];
} st_request;
typedef st_request* request;

typedef struct req_process {
    int client_id;
    int request_type;
    char data[MAX_DATA_LENGTH];
}req_process;
typedef req_process* proc;

typedef struct st_response{
    int response_type;
    char message[MAX_REQUEST_LEN];
    char IP_Addr[16];
    int Port_No;
} st_response;
typedef st_response* response;

typedef struct file_info{
    char file_name[MAX_FILE_NAME];
    char file_path[MAX_PATH_LEN];
    int file_size;
    bool is_audio;
    bool is_dir;
    time_t last_access_time;
    time_t last_modified_time;
} file_info;

// Global Variables
extern client client_list[MAX_CLIENTS];
extern ss storage_server_list[100];
extern int socket_arr[MAX_CONNECTIONS][2];
extern int storage_server_count;
// Functions for handling requests
void *work_handler();
void handle_file_request(request req, int client_id);




// Hashing
extern int* primes;

typedef struct st_node{
    char* path;   // path
    int len;   // length of the path entered
    int s_index;     // index of the storage server
    struct st_node* next;
}st_node;

typedef st_node* node;

typedef struct compress{
    char* a;
    long long int hashval;
}compress;

extern node* hashtable;
node* Create_hastable(int itablesize);
int isPrime(int x);
int findnextprime(int x);
long long int create_hash(char* x, int* primes, int n);
void Insert(char* path, int l, int pos, node* hashtable, int s_i);
int get(node* hashtable, char* cmp, int pos);
void Delete(char* path, int pos, node* hashtable);




// Logging 
typedef struct LogEntry {
    time_t timestamp;             // The time of the log entry
    bool isClient;                // True if the log entry is for a client, false if for a SS
    char IP_Addr[INET_ADDRSTRLEN];  // IP address of the client or SS
    int Port_No;               // Port number of the client or SS
    st_request request;         // Request made by the client
    int response_code;          // Response code of the request
} LogEntry;

void logMessage(bool isClient, int client_socket, st_request request, int response_code);


#include "cache_handling.h"

#endif