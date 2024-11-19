#include "functions.h"

int copy_file(st_request* req) {
    printf("at copy file Copying file: %s\n", req->src_path);
    int src_fd;
    char buffer[1024];
    ssize_t bytes_read, bytes_sent;

    // Connect to destination server
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(req->port_for_copy);
    if (inet_pton(AF_INET, req->ip_for_copy, &dest_addr.sin_addr) < 0) {
        perror("Invalid destination IP address");
        return -1;
    }
    printf("ip for copy-%s\n", req->ip_for_copy);
    printf("port for copy-%d\n", req->port_for_copy);

    // Create new socket for connection
    int copy_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (copy_sock < 0) {
        perror("Failed to create socket");
        return -1;
    }

    if (connect(copy_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Failed to connect to destination server");
        close(copy_sock);
        return -1;
    }

    // Open source file
    src_fd = open(req->src_path, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        st_response error;
        error.response_type = FILE_COPY_ERROR;
        strcpy(error.message, "Error: Unable to open the source file for copying\n");
        send(req->socket, &error, sizeof(st_response), 0);
        close(copy_sock);
        return -1;
    }

    // Send file data using the same socket used for connection
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_sent = send(copy_sock, buffer, bytes_read, 0);
        if (bytes_sent < 0) {
            perror("Failed to send file data");
            close(src_fd);
            close(copy_sock);
            return -1;
        }
    }

    close(src_fd);
    close(copy_sock);
    printf("File copy complete: %s\n", req->src_path);
    return 0;
}

// int copy_directory_recursive(const char* src_path, const char* dest_path, int copy_sock) {
//     DIR* dir = opendir(src_path);
//     if (!dir) {
//         perror("Failed to open source directory");
//         return -1;
//     }

//     struct dirent* entry;
//     while ((entry = readdir(dir))) {
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
//             continue;
//         }

//         char src_entry_path[MAX_PATH_LEN];
//         snprintf(src_entry_path, sizeof(src_entry_path), "%s/%s", src_path, entry->d_name);

//         // Check if file exists and is accessible
//         if (access(src_entry_path, F_OK) < 0) {
//             printf("Skipping non-existent file: %s\n", src_entry_path);
//             continue;
//         }

//         if (access(src_entry_path, R_OK) < 0) {
//             printf("Skipping unreadable file: %s\n", src_entry_path);
//             continue;
//         }

//         struct stat stat_buf;
//         if (stat(src_entry_path, &stat_buf) < 0) {
//             continue;
//         }

//         // Additional check for regular file or directory
//         if (!S_ISREG(stat_buf.st_mode) && !S_ISDIR(stat_buf.st_mode)) {
//             printf("Skipping non-regular file: %s\n", src_entry_path);
//             continue;
//         }

//         // Send file/directory info
//         file_info info;
//         strncpy(info.file_name, entry->d_name, MAX_FILE_NAME - 1);
//         info.is_dir = S_ISDIR(stat_buf.st_mode);
//         info.file_size = stat_buf.st_size;

//         if (send(copy_sock, &info, sizeof(info), 0) < 0) {
//             perror("Failed to send file info");
//             closedir(dir);
//             return -1;
//         }

//         if (info.is_dir) {
//             // Recursively copy subdirectory using same socket
//             char new_dest_path[MAX_PATH_LEN];
//             snprintf(new_dest_path, sizeof(new_dest_path), "%s/%s", dest_path, entry->d_name);
//             if (copy_directory_recursive(src_entry_path, new_dest_path, copy_sock) < 0) {
//                 closedir(dir);
//                 return -1;
//             }
//         } else {
//             // Copy regular file
//             int src_fd = open(src_entry_path, O_RDONLY);
//             if (src_fd < 0) continue;

//             char buffer[1024];
//             ssize_t bytes_read;
//             while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
//                 if (send(copy_sock, buffer, bytes_read, 0) < 0) {
//                     perror("Failed to send file data");
//                     close(src_fd);
//                     closedir(dir);
//                     return -1;
//                 }
//                 memset(buffer, 0, sizeof(buffer));
//             }
//             close(src_fd);
//         }
//     }

//     closedir(dir);
//     return 0;
// }

// int copy_directory(st_request* req) {
//     printf("copy dir req->type: %d\n", req->request_type);
//     printf("Copying directory from: %s\n", req->src_path);
    
//     // Connect to destination server
//     struct sockaddr_in dest_addr;
//     dest_addr.sin_family = AF_INET;
//     dest_addr.sin_port = htons(req->port_for_copy);
//     if (inet_pton(AF_INET, req->ip_for_copy, &dest_addr.sin_addr) < 0) {
//         perror("Invalid destination IP address");
//         return -1;
//     }

//     // Create new socket for connection
//     int copy_sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (copy_sock < 0) {
//         perror("Failed to create socket");
//         return -1;
//     }

//     if (connect(copy_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
//         perror("Failed to connect to destination server");
//         close(copy_sock);
//         return -1;
//     }

//     // Start recursive copy using the single socket
//     int result = copy_directory_recursive(req->src_path, req->dest_path, copy_sock);
    
//     close(copy_sock);
//     return result;
// }


int copy_directory(st_request* req) {
    printf("copy dir req->type: %d\n", req->request_type);
    printf("Copying directory from: %s\n", req->src_path);
    
    // Connect to destination server
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(req->port_for_copy);
    if (inet_pton(AF_INET, req->ip_for_copy, &dest_addr.sin_addr) < 0) {
        perror("Invalid destination IP address");
        return -1;
    }

    // Create new socket for connection
    int copy_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (copy_sock < 0) {
        perror("Failed to create socket");
        return -1;
    }

    if (connect(copy_sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Failed to connect to destination server");
        close(copy_sock);
        return -1;
    }

    DIR* dir = opendir(req->src_path);
    if (!dir) {
        perror("Failed to open source directory");
        close(copy_sock);
        return -1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char src_entry_path[MAX_PATH_LEN];
        snprintf(src_entry_path, sizeof(src_entry_path), "%s/%s", req->src_path, entry->d_name);

        struct stat stat_buf;
        if (stat(src_entry_path, &stat_buf) < 0) {
            continue;
        }

        // Send file/directory info
        file_info info;
        strncpy(info.file_name, entry->d_name, MAX_FILE_NAME - 1);
        info.is_dir = S_ISDIR(stat_buf.st_mode);
        info.file_size = stat_buf.st_size;

        if (send(copy_sock, &info, sizeof(info), 0) < 0) {
            perror("Failed to send file info");
            close(copy_sock);
            closedir(dir);
            return -1;
        }

        if (info.is_dir) {
            // Recursively copy subdirectory
            st_request subreq = *req;
            strcpy(subreq.src_path, src_entry_path);
            printf("path for recursive copy:%s\n", src_entry_path);
            if (copy_directory(&subreq) < 0) {
                close(copy_sock);
                closedir(dir);
                return -1;
            }
        } else {
            // Copy regular file
            int src_fd = open(src_entry_path, O_RDONLY);
            if (src_fd < 0) continue;

            char buffer[1024];
            ssize_t bytes_read;
            while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                if (send(copy_sock, buffer, bytes_read, 0) < 0) {
                    perror("Failed to send file data");
                    close(src_fd);
                    close(copy_sock);
                    closedir(dir);
                    return -1;
                }
                memset(buffer, 0, sizeof(buffer));
            }
            close(src_fd);
        }
    }

    closedir(dir);
    close(copy_sock);
    return 0;
}