// typedef struct request{
//     int request_type;
//     char data[MAX_REQUEST_LEN];
//     char src_path[MAX_PATH_LEN];
//     char src_file_dir_name[MAX_FILE_NAME];
//     char dest_path[MAX_PATH_LEN];
//     char ip_for_copy[IP_ADDR_LEN];  // IP of the storage server to copy the file to
//     int port_for_copy;  // Port of the storage server to copy the file to
//     int socket;
// } st_request;

#include "functions.h"

extern int storage_server_socket;

int copy_file(st_request* req) {
    int src_fd, dest_sock;
    char buffer[1024];
    ssize_t bytes_read, bytes_sent;

    // Open the source file for reading
    char src_file[MAX_PATH_LEN];
    // snprintf(src_file, MAX_PATH_LEN, "%s/%s", req->src_path, req->src_file_dir_name);
    strcpy(src_file, req->src_path);

    src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        st_response error;
        error.response_type=FILE_COPY_ERROR;
        strcpy(error.message, "Error: Unable to open the source file for copying\n");
        if(send(req->socket, &error, sizeof(st_response), 0) < 0){
            perror("Failed to send error response to client");
        }
        else{
            printf("Error response sent to client\n");
        }
        // return -1;
    }

    // Set up the destination server socket
    struct sockaddr_in dest_addr;
    dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock < 0) {
        perror("Failed to create destination socket");
        close(src_fd);
        st_response error;
        error.response_type=FILE_COPY_ERROR;
        strcpy(error.message, "Error: Unable to create destination socket\n");
        if(send(req->socket, &error, sizeof(st_response), 0) < 0){
            perror("Failed to send error response to client");
        }
        else{
            printf("Error response sent to client\n");
        }
        return -1;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(req->port_for_copy);
    if (inet_pton(AF_INET, req->ip_for_copy, &dest_addr.sin_addr) < 0) {
        perror("Invalid destination IP address");
        close(src_fd);
        close(dest_sock);
        st_response error;
        error.response_type=FILE_COPY_ERROR;
        strcpy(error.message, "Error: Invalid destination IP address\n");
        if(send(req->socket, &error, sizeof(st_response), 0) < 0){
            perror("Failed to send error response to client");
        }
        else{
            printf("Error response sent to client\n");
        }
        return -1;
    }

    // Connect to the destination server
    if (connect(dest_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Failed to connect to destination server");
        close(src_fd);
        close(dest_sock);
        st_response error;
        error.response_type=FILE_COPY_ERROR;
        strcpy(error.message, "Error: Failed to connect to destination server\n");
        if(send(req->socket, &error, sizeof(st_response), 0) < 0){
            perror("Failed to send error response to client");
        }
        else{
            printf("Error response sent to client\n");
        }
        return -1;
    }

    // Inform the destination server about the incoming file
    // char dest_file[MAX_PATH_LEN];
    // snprintf(dest_file, MAX_PATH_LEN, "%s/%s", req->dest_path, req->src_file_dir_name);

    // if (send(dest_sock, dest_file, strlen(dest_file) + 1, 0) < 0) {
    //     perror("Failed to send destination file path");
    //     close(src_fd);
    //     close(dest_sock);
    //     return -1;
    // }

    // Read from the source file and send to the destination server

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        ssize_t total_bytes_sent = 0;
        while (total_bytes_sent < bytes_read) {
            bytes_sent = send(dest_sock, buffer + total_bytes_sent, bytes_read - total_bytes_sent, 0);
            if (bytes_sent < 0) {
                perror("Failed to send file data");
                close(src_fd);
                close(dest_sock);
                return -1;
            }
            total_bytes_sent += bytes_sent;
        }
    }

    if (bytes_read < 0) {
        perror("Failed to read from source file");
        close(src_fd);
        close(dest_sock);
        return -1;
    }
    // while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
    //     bytes_sent = send(dest_sock, buffer, bytes_read, 0);
    //     if (bytes_sent < 0) {
    //         perror("Failed to send file data");
    //         close(src_fd);
    //         close(dest_sock);

    //         return -1;
    //         // break;
    //     }
    // }

    // if (bytes_read < 0) {
    //     perror("Error reading from source file");
    //     close(src_fd);
    //     close(dest_sock);
    //     return -1;
    // }

    // Clean up resources
    close(src_fd);
    close(dest_sock);

    printf("File copy operation complete for file: %s\n", req->src_file_dir_name);
    return 0;
}

int copy_directory(st_request* req) {
    DIR* dir;
    struct dirent* entry;
    struct stat entry_stat;

    char src_dir[MAX_PATH_LEN];
    // snprintf(src_dir, MAX_PATH_LEN, "%s/%s", req->src_path, req->src_file_dir_name);
    // Construct the full path
    // if (strcmp(req->src_path, "/") == 0) {
    //     // snprintf(full_path, sizeof(full_path), "main/%s", req->src_file_dir_name);
    //     strcpy(src_dir, "main/");
    //     strcat(src_dir, req->src_file_dir_name);
    // } else {
    //     // snprintf(full_path, sizeof(full_path), "main/%s/%s", req->src_path, req->src_file_dir_name);
    //     strcpy(src_dir, "main/");
    //     strcat(src_dir, req->src_path);
    //     strcat(src_dir, "/");
    //     strcat(src_dir, req->src_file_dir_name);
    // }
    strcpy(src_dir, req->src_path);

    // Set up the destination server socket
    struct sockaddr_in dest_addr;
    int dest_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (dest_sock < 0) {
        perror("Failed to create destination socket");
        return -1;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(req->port_for_copy);
    if (inet_pton(AF_INET, req->ip_for_copy, &dest_addr.sin_addr) < 0) {
        perror("Invalid destination IP address");
        close(dest_sock);
        return -1;
    }

    // Connect to the destination server
    if (connect(dest_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Failed to connect to destination server");
        close(dest_sock);
        return -1;
    }

    // Inform the destination server about the incoming directory
    if (send(dest_sock, req->dest_path, strlen(req->dest_path) + 1, 0) < 0) {
        perror("Failed to send destination directory path");
        close(dest_sock);
        return -1;
    }

    // Open the source directory
    dir = opendir(src_dir);
    if (!dir) {
        perror("Failed to open source directory");
        close(dest_sock);
        return -1;
    }

    // Iterate through directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct paths for the source and destination
        char src_entry_path[MAX_PATH_LEN];
        snprintf(src_entry_path, MAX_PATH_LEN, "%s/%s", src_dir, entry->d_name);

        char dest_entry_path[MAX_PATH_LEN];
        snprintf(dest_entry_path, MAX_PATH_LEN, "%s/%s", req->dest_path, entry->d_name);

        // Get entry information
        if (stat(src_entry_path, &entry_stat) < 0) {
            perror("Failed to get entry stats");
            continue;
        }

        if (S_ISREG(entry_stat.st_mode)) {
            // Send file
            int src_fd = open(src_entry_path, O_RDONLY);
            if (src_fd < 0) {
                perror("Failed to open source file");
                continue;
            }

            char buffer[1024];
            ssize_t bytes_read, bytes_sent;
            while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                bytes_sent = send(dest_sock, buffer, bytes_read, 0);
                if (bytes_sent < 0) {
                    perror("Failed to send file data");
                    close(src_fd);
                    close(dest_sock);
                    closedir(dir);
                    return -1;
                }
            }

            if (bytes_read < 0) {
                perror("Error reading from source file");
            }

            close(src_fd);
        } else if (S_ISDIR(entry_stat.st_mode)) {
            // Recursively copy subdirectory
            st_request sub_req = *req;
            strcpy(sub_req.src_path, src_dir);
            strcpy(sub_req.src_file_dir_name, entry->d_name);
            strcpy(sub_req.dest_path, dest_entry_path);
            if (copy_directory(&sub_req) < 0) {
                perror("Failed to copy subdirectory");
                close(dest_sock);
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);
    close(dest_sock);

    printf("Directory copy operation complete for directory: %s\n", req->src_file_dir_name);
    return 0;
}
