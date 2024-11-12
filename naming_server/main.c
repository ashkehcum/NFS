#include<stdio.h>
#include"headers.h"
// Naming Server
// Naming Server is responsible for keeping track of all the storage servers and their status.

int main(){
    printf("Naming Server Started\n");

    pthread_t working_thread;

    pthread_create(&working_thread, NULL, &work_handler, NULL);
    

    return 0;
}