#include "functions.h"

int receive_file(st_request* req, int client_sock)
{
    char buffer[1024];
    ssize_t bytes_received;

    // Receive the destination file path
    char dest_file[MAX_PATH_LEN];
    bytes_received = recv(client_sock, dest_file, sizeof(dest_file), 0);
    if (bytes_received <= 0)
    {
        perror("Failed to receive destination file path");
        close(client_sock);
        return -1;
    }
    dest_file[bytes_received] = '\0'; // Null-terminate the string

    // Open the destination file for writing
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd < 0)
    {
        perror("Failed to open destination file");
        close(client_sock);
        return -1;
    }

    // Receive the file data and write to the destination file
    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        if (write(dest_fd, buffer, bytes_received) < 0)
        {
            perror("Failed to write to destination file");
            close(dest_fd);
            close(client_sock);
            return -1;
        }
    }

    if (bytes_received < 0)
    {
        perror("Error receiving file data");
    }

    // Clean up resources
    close(dest_fd);
    close(client_sock);

    printf("File received and saved to: %s\n", dest_file);
    return 0;
}

#include "functions.h"

int receive_directory(st_request* req, int client_sock)
{
    char buffer[1024];
    ssize_t bytes_received;

    // Receive the destination directory path
    char dest_dir[MAX_PATH_LEN];
    bytes_received = recv(client_sock, dest_dir, sizeof(dest_dir), 0);
    if (bytes_received <= 0)
    {
        perror("Failed to receive destination directory path");
        close(client_sock);
        return -1;
    }
    dest_dir[bytes_received] = '\0'; // Null-terminate the string

    // Create the destination directory
    if (mkdir(dest_dir, 0755) < 0 && errno != EEXIST) {
        perror("Failed to create destination directory");
        close(client_sock);
        return -1;
    }

    // Receive the directory entries and files
    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0)
    {
        // Process the received data (e.g., save files, create subdirectories)
        // This part depends on the protocol you use to send the directory structure and files
        // For simplicity, let's assume you send file paths followed by file data

        // Example: Receive file path
        char file_path[MAX_PATH_LEN];
        strncpy(file_path, buffer, bytes_received);
        file_path[bytes_received] = '\0';

        // Open the destination file for writing
        int dest_fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (dest_fd < 0)
        {
            perror("Failed to open destination file");
            close(client_sock);
            return -1;
        }

        // Receive the file data and write to the destination file
        while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0)
        {
            if (write(dest_fd, buffer, bytes_received) < 0)
            {
                perror("Failed to write to destination file");
                close(dest_fd);
                close(client_sock);
                return -1;
            }
        }

        if (bytes_received < 0)
        {
            perror("Error receiving file data");
        }

        // Clean up resources
        close(dest_fd);
    }

    close(client_sock);

    printf("Directory received and saved to: %s\n", dest_dir);
    return 0;
}