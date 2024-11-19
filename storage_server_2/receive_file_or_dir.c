#include "functions.h"

int receive_file(st_request* req, int client_sock) {
    printf("Receiving file at path: %s\n", req->dest_path);
    printf("rec file req->src_file_dir_name: %s\n", req->src_file_dir_name);

    char buffer[1024];
    ssize_t bytes_received;
    char full_path[MAX_PATH_LEN];
    strcpy(full_path, req->dest_path);
    strcat(full_path, "/");
    strcat(full_path, req->src_file_dir_name);
    printf("receiver full path:%s\n", full_path);
    
    // Open destination file for writing
    int dest_fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0) {
        perror("Failed to open destination file");
        return -1;
    }

    // Receive and write file data
    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        if (write(dest_fd, buffer, bytes_received) < 0) {
            perror("Failed to write to file");
            close(dest_fd);
            return -1;
        }
    }

    if (bytes_received < 0) {
        perror("Error receiving file data");
        close(dest_fd);
        return -1;
    }

    // Send acknowledgment to nag server
    // st_response ack;
    // ack.response_type = ACK_MSG;
    // if (send(ns_sock, &ack, sizeof(ack), 0) < 0) {
    //     perror("Failed to send ACK to Naming Server");
    // }
    // else {
    //     printf("ACK sent to Naming Server\n");
    // }

    close(dest_fd);
    printf("File received successfully at: %s\n", req->dest_path);
    return 0;
}
int receive_directory(st_request* req, int client_sock) {
    printf("rec dir Receiving directory at: %s\n", req->dest_path);
    printf("rec dir req->src_file_dir_name: %s\n", req->src_file_dir_name);

    char buffer[1024];
    ssize_t bytes_received;

    // Create base destination directory
    char full_path[MAX_PATH_LEN];
    // strcpy(full_path, req->dest_path);
    // strcat(full_path, "/");
    // strcat(full_path, req->src_file_dir_name);
    if(strcmp(req->src_file_dir_name, "b1")==0 || strcmp(req->src_file_dir_name, "b2")==0 || strcmp(req->src_file_dir_name, "b3")==0){
        strcpy(full_path, req->src_file_dir_name);
        printf("Full path for backup:%s\n", full_path);
    }
    else{
        strcpy(full_path, req->dest_path);
        strcat(full_path, "/");
        strcat(full_path, req->src_file_dir_name);
        printf("full path for normal receive %s\n", full_path);
    }
    
    printf("full path before receiving:%s\n", full_path);
    
    if (mkdir(full_path, 0755) < 0 && errno != EEXIST) {
        perror("Failed to create destination directory");
        return -1;
    }

    while (1) {
        // Receive file/directory info
        file_info info;
        bytes_received = recv(client_sock, &info, sizeof(info), 0);
        if (bytes_received <= 0) break;  // End of transmission

        char entry_path[MAX_COPY_LEN];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", full_path, info.file_name);
        printf("entry path:%s\n", entry_path);

        if (info.is_dir) {
            if (mkdir(entry_path, 0755) < 0 && errno != EEXIST) {
                perror("Failed to create subdirectory");
                return -1;
            }
            printf("Created directory: %s\n", entry_path);
        } else {
            // Receive and write file
            int dest_fd = open(entry_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dest_fd < 0) {
                perror("Failed to create file");
                return -1;
            }

            size_t remaining = info.file_size;
            while (remaining > 0) {
                bytes_received = recv(client_sock, buffer, 
                                   (sizeof(buffer), remaining), 0);
                if (bytes_received <= 0) {
                    perror("Failed to receive file data");
                    close(dest_fd);
                    return -1;
                }

                if (write(dest_fd, buffer, bytes_received) < 0) {
                    perror("Failed to write file data");
                    close(dest_fd);
                    return -1;
                }
                remaining -= bytes_received;
            }
            close(dest_fd);
            printf("Received file: %s\n", entry_path);
        }
    }

    printf("Directory received successfully at: %s\n", full_path);
    return 0;
}

