#include "functions.h"

void create_file_or_directory(st_request* req) {
    char full_path[2*MAX_PATH_LEN];
    memset(full_path, 0, sizeof(full_path));

    // Construct the full path
    snprintf(full_path, sizeof(full_path), "%s/%s", req->path, req->file_or_dir_name);

    if (req->request_type ==CREATE_FILE) { // Create file
        int fd = open(full_path, O_CREAT | O_WRONLY, 0644); // rw-r--r-- permissions
        if (fd < 0) {
            perror("Failed to create file");
            const char *error_msg = "Error: Unable to create file\n";
            send(req->socket, error_msg, strlen(error_msg), 0);
        } else {
            printf("File created: %s\n", full_path);
            close(fd);

            const char *success_msg = "File created successfully\n";
            send(req->socket, success_msg, strlen(success_msg), 0);
        }
    } else if (req->request_type == CREATE_DIR) { // Create directory
        if (mkdir(full_path, 0755) < 0) { // rwxr-xr-x permissions
            perror("Failed to create directory");
            const char *error_msg = "Error: Unable to create directory\n";
            send(req->socket, error_msg, strlen(error_msg), 0);
        } else {
            printf("Directory created: %s\n", full_path);
            const char *success_msg = "Directory created successfully\n";
            send(req->socket, success_msg, strlen(success_msg), 0);
        }
    } else {
        const char *invalid_msg = "Invalid request type. Use 1 for file, 2 for directory.\n";
        send(req->socket, invalid_msg, strlen(invalid_msg), 0);
    }

    // Send acknowledgment
    const char *ack_msg = "ACK";
    if (send(req->socket, ack_msg, strlen(ack_msg), 0) < 0) {
        perror("Failed to send ACK to client");
    } else {
        printf("ACK sent to client.\n");
    }
}