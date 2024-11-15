#include "functions.h"

void get_file_info(st_request* req) {
    struct stat file_stat;
    char response[1024]; // Buffer to hold the response
    memset(response, 0, sizeof(response));

    // Get file information using stat()
    if (stat(req->path, &file_stat) < 0) {
        perror("Failed to retrieve file information");
        snprintf(response, sizeof(response), "Error: Unable to retrieve file information for %s\n", req->path);
        send(req->socket, response, strlen(response), 0);
        return;
    }

    // Retrieve file size
    off_t file_size = file_stat.st_size;

    // Retrieve file permissions
    mode_t permissions = file_stat.st_mode;

    // Format file information into a response string
    snprintf(response, sizeof(response),
             "File: %s\nSize: %ld bytes\nPermissions: %o (octal)\n",
             req->path, file_size, permissions & 0777); // Mask with 0777 to get only permission bits

    // Send the file information to the client
    if (send(req->socket, response, strlen(response), 0) < 0) {
        perror("Failed to send file information to client");
        return;
    }

    printf("File information sent to client: %s\n", response);

    // Send acknowledgment to the client
    const char *ack_msg = "ACK";
    if (send(req->socket, ack_msg, strlen(ack_msg), 0) < 0) {
        perror("Failed to send ACK to client");
    } else {
        printf("ACK sent to client.\n");
    }
}