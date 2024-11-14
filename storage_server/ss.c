#include "functions.h"

// void initialize_accessible_paths(int argc, char *argv[], char (*accessible_paths)[MAX_PATH_LEN], char* accessible_path){
//     for(int i=2; i<argc; i++){
//         int path_no=0;
//         strncpy(accessible_paths[path_no++], argv[i], MAX_PATH_LEN);
//         // accessible_paths[i][MAX_PATH_LEN-1]='\0'; // Ensure null termination
//         strcpy(accessible_path, )
//     }
// }

void initialize_accessible_paths(int argc, char* argv[], char* accessible_path){
    for(int i=3; i<argc; i++){
        strcat(accessible_path, argv[i]);
        strcat(accessible_path, " ");
    }
}

int main(int argc, char *argv[]){
    // char accessible_paths[MAX_PATHS][MAX_PATH_LEN];
    storage_server ss1;
    char ip[IP_ADDR_LEN];
    char nm_ip[IP_ADDR_LEN];
    int ss_port_no;
    // get_ip_address(ip);
    if(get_ip_and_port(ip, &ss_port_no)==-1){
        perror("Failed to get ip and port no of storage_server\n");
        return(EXIT_FAILURE);
    }
    printf("%s %d\n", ip, ss_port_no);
    strncpy(ss1.IP_Addr, ip, sizeof(ss1.IP_Addr));
    ss1.Port_No=ss_port_no;
    // ss1.NM_Port=atoi(argv[1]);
    
    strcpy(nm_ip, argv[1]);
    strcpy(ss1.nm_ip, nm_ip);
    ss1.NM_Port=atoi(argv[2]);

    char accessible_paths[MAX_PATHS*MAX_PATH_LEN];
    // for(int i=3; i<argc; i++){
    //     strcat(accessible_paths, argv[i]);
    //     strcat(accessible_paths, " ");
    // }
    int k;
    printf("Enter no of accessible paths: ");
    scanf("%d", &k);
    for(int i=0; i<k; i++){
        char path[MAX_PATH_LEN];
        printf("Enter path %d: ", i+1);
        scanf("%s", path);
        strcat(accessible_paths, path);
        strcat(accessible_paths, ";");
    }
    printf("%s\n", accessible_paths);
    
    // ss1.Client_Port=atoi(argv[2]);
    // char accessible_paths[MAX_PATHS*MAX_PATH_LEN];

    // initialize_accessible_paths(argc, argv, accessible_paths);

    if(register_with_naming_server(&ss1, accessible_paths)<0){
        fprintf(stderr, "Failed to register with Naming Server\n");
        exit(1);
    }

    // Starting threads for listening to Naming Server and Clients
    pthread_t ns_thread, client_thread;
    // pthread_create(&ns_thread, NULL, naming_server_listener, (void *)&ss1);
    pthread_create(&client_thread, NULL, client_listener, (void *)&ss_port_no);

    // Wait for threads to finish
    // pthread_join(ns_thread, NULL);
    pthread_join(client_thread, NULL);

    return 0;
}