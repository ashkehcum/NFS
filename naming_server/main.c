#include<stdio.h>
#include"headers.h"
// Naming Server
// Naming Server is responsible for keeping track of all the storage servers and their status.

int main(){
    printf("Naming Server Started\n");

    pthread_t working_thread;
    storage_server_count = 0;
    pthread_create(&working_thread, NULL, &work_handler, NULL);
    

    return 0;
}