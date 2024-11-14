#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void get_ip_address(char* ip_addr, int* port) {
    int sock;
    struct sockaddr_in server;
    char ip[INET_ADDRSTRLEN];

    // Create a socket for this function
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return;
    }

    // Using an arbitrary address to determine the local IP
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("8.8.8.8"); // Google DNS
    server.sin_port = htons(80);

    // Connect the socket
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Connection failed");
        close(sock);
        return;
    }

    // Get the local IP address
    struct sockaddr_in local_address;
    socklen_t address_length = sizeof(local_address);
    if (getsockname(sock, (struct sockaddr *)&local_address, &address_length) == -1) {
        perror("getsockname failed");
        close(sock);
        return;
    }

    // Convert the IP to a readable string
    inet_ntop(AF_INET, &local_address.sin_addr, ip, sizeof(ip));
    // printf("Your IP address: %s\n", ip);  // Print the IP address
    strcpy(ip_addr, ip);

    *port=ntohs(local_address.sin_port);
    // printf("%d\n", port);

    // Close the socket
    close(sock);
}

int main() {
    char ip_addr[16];
    int port;
    get_ip_address(ip_addr, &port);
    printf("%s %d\n", ip_addr, port);
    return 0;
}
