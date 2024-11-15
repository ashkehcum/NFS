#include "functions.h"

void delete_file_or_directory(st_request* req) {
    char full_path[2*MAX_PATH_LEN];
    memset(full_path, 0, sizeof(full_path));

    // Construct the full path
    snprintf(full_path, sizeof(full_path), "%s/%s", req->path, req->file_or_dir_name);

    // Check if the file/directory exists
    struct stat path_stat;
    if (stat(full_path, &path_stat) < 0) {
        perror("Stat failed");
        const char *error_msg = "Error: File or directory not found\n";
        send(req->socket, error_msg, strlen(error_msg), 0);
        return;
    }

    if (S_ISREG(path_stat.st_mode)) {
        // Delete file
        if (unlink(full_path) < 0) {
            perror("Failed to delete file");
            const char *error_msg = "Error: Unable to delete file\n";
            send(req->socket, error_msg, strlen(error_msg), 0);
        } else {
            printf("File deleted: %s\n", full_path);
            const char *success_msg = "File deleted successfully\n";
            send(req->socket, success_msg, strlen(success_msg), 0);
        }
    } else if (S_ISDIR(path_stat.st_mode)) {
        // Delete directory
        if (rmdir(full_path) < 0) {
            perror("Failed to delete directory");
            const char *error_msg = "Error: Unable to delete directory\n";
            send(req->socket, error_msg, strlen(error_msg), 0);
        } else {
            printf("Directory deleted: %s\n", full_path);
            const char *success_msg = "Directory deleted successfully\n";
            send(req->socket, success_msg, strlen(success_msg), 0);
        }
    } else {
        const char *invalid_msg = "Error: Path is neither a file nor a directory\n";
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