#include<stdio.h>
#include"headers.h"

// Naming Server
// Naming Server is responsible for keeping track of all the storage servers and their status.
node* hashtable;
my_cache* cache;

pthread_mutex_t hashLock;
pthread_mutex_t cacheLock;
pthread_mutex_t logLock;

void initlocks(){
    pthread_mutex_init(&hashLock, NULL);
    pthread_mutex_init(&cacheLock, NULL);
    pthread_mutex_init(&logLock, NULL);
}

void destroylocks(){
    pthread_mutex_destroy(&hashLock);
    pthread_mutex_destroy(&cacheLock);
    pthread_mutex_destroy(&logLock);
}

int main(){
    printf("Naming Server Started\n");

    pthread_t working_thread, backup_thread, ping_thread;
    hashtable = Create_hashtable();
    cache = initialize_cache();
    initlocks();

    pthread_create(&working_thread, NULL, &work_handler, NULL);
    pthread_create(&backup_thread, NULL, &backup_handler, NULL);
    // pthread_create(&ping_thread, NULL, &storage_server_handler, NULL);



    pthread_join(working_thread, NULL);
    pthread_join(backup_thread, NULL);

    Free_hashtable();
    Free_cache();

    destroylocks();

    while(1);
    return 0;
}