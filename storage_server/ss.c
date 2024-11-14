#include "functions.h"

void initialize_accessible_paths(int argc, char *argv[], char (*accessible_paths)[MAX_PATH_LEN]){
    for(int i=0; i<argc-3; i++){
        strncpy(accessible_paths[i], argv[i+3], MAX_PATH_LEN);
        accessible_paths[i][MAX_PATH_LEN-1]='\0'; // Ensure null termination
    }
}

int main(int argc, char *argv[]){
    char accessible_paths[MAX_PATHS][MAX_PATH_LEN];
    storage_server ss1;
    strncpy(ss1.IP_Addr, "127.0.0.1", sizeof(ss1.IP_Addr));
    // ss1.NM_Port=atoi(argv[1]);
    ss1.Port_No=atoi(argv[1]);
    ss1.Client_Port=atoi(argv[2]);
    initialize_accessible_paths(argc, argv, accessible_paths);

    if(register_with_naming_server(&ss1, accessible_paths)<0){
        fprintf(stderr, "Failed to register with Naming Server\n");
        exit(1);
    }

    // Starting threads for listening to Naming Server and Clients
    pthread_t ns_thread, client_thread;
    pthread_create(&ns_thread, NULL, naming_server_listener, (void *)&ss1);
    pthread_create(&client_thread, NULL, client_listener, (void *)&ss1);

    // Wait for threads to finish
    pthread_join(ns_thread, NULL);
    pthread_join(client_thread, NULL);

    return 0;
}