#include<stdio.h>
#include"headers.h"

// Naming Server
// Naming Server is responsible for keeping track of all the storage servers and their status.
node* hashtable;
my_cache* cache;

int main(){
    printf("Naming Server Started\n");

    pthread_t working_thread;
    hashtable = Create_hashtable();
    cache = initialize_cache();

    pthread_create(&working_thread, NULL, &work_handler, NULL);
    pthread_join(working_thread, NULL);

    while(1);
    return 0;
}