#include "functions.h"

// int get_ip_and_port(char* ip_addr, int* port) {
//     int sock;
//     struct sockaddr_in server;
//     char ip[INET_ADDRSTRLEN];

//     // Create a socket for this function
//     sock = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sock == -1) {
//         perror("Socket creation failed");
//         return -1;
//     }

//     // Using an arbitrary address to determine the local IP
//     server.sin_family = AF_INET;
//     server.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS
//     server.sin_port = htons(80);

//     // Connect the socket
//     if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
//         perror("Connection failed");
//         close(sock);
//         return -1;
//     }

//     // Get the local IP address
//     struct sockaddr_in local_address;
//     socklen_t address_length = sizeof(local_address);
//     if (getsockname(sock, (struct sockaddr *)&local_address, &address_length) == -1) {
//         perror("getsockname failed");
//         close(sock);
//         return -1;
//     }

//     // Convert the IP to a readable string
//     *port = ntohs(local_address.sin_port);

    
//     // printf("Your IP address: %s\n", ip);  // Print the IP address
//     strcpy(ip_addr, ip);

//     // Close the socket
//     close(sock);
//     return 0;
// }

int get_ip_and_port(char* ip_addr, int* port){
        int sock;
    struct sockaddr_in server;
    char ip[INET_ADDRSTRLEN];

    // Create a socket for this function
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Using an arbitrary address to determine the local IP
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS
    server.sin_port = htons(80);

    // Connect the socket
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connection failed");
        close(sock);
        return -1;
    }

    // Get the local IP address
    struct sockaddr_in local_address;
    socklen_t address_length = sizeof(local_address);
    if (getsockname(sock, (struct sockaddr *)&local_address, &address_length) == -1) {
        perror("getsockname failed");
        close(sock);
        return -1;
    }

    // Convert the IP to a readable string
    inet_ntop(AF_INET, &local_address.sin_addr, ip, sizeof(ip));
    // printf("Your IP address: %s\n", ip);  // Print the IP address
    strcpy(ip_addr, ip);

    *port=ntohs(local_address.sin_port);
    // printf("%d\n", port);

    // Close the socket
    close(sock);
    return 0;
}