#include<stdio.h>
#include"headers.h"

// Naming Server
// Naming Server is responsible for keeping track of all the storage servers and their status.
node* hashtable;
int* primes;
void initialise()
{
    hashtable = Create_hashtable(196613);
}
int main(){
    printf("Naming Server Started\n");

    pthread_t working_thread;
    // storage_server_count = 0;
    initialise();
    pthread_create(&working_thread, NULL, &work_handler, NULL);
    pthread_join(working_thread, NULL);
    while(1);

    return 0;
}