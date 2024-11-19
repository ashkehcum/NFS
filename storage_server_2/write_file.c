#include "functions.h"
#define LIMIT 10
int write_file(st_request *req)
{
    int file_fd;
    ssize_t bytes_written;
    size_t data_len = strlen(req->data);

    char full_path[2 * MAX_PATH_LEN];
    memset(full_path, 0, sizeof(full_path));
    printf("Source path: %s\n", req->src_path);
    printf("Source file/dir name: %s\n", req->src_file_dir_name);
    snprintf(full_path, sizeof(full_path), "main/%s", req->src_path);
    // if (strcmp(req->src_path, "/") == 0) {
    //     snprintf(full_path, sizeof(full_path), "main/%s", req->src_file_dir_name);
    // } else {
    //     snprintf(full_path, sizeof(full_path), "main/%s/%s", req->src_path, req->src_file_dir_name);
    // }
    if (req->sync == 1 || strlen(req->data) > LIMIT)
    {
        st_request ack;
        ack.request_type = ACK_MSG;
        // const char *ack_msg = "ACK";
        if (send(req->socket, &ack, sizeof(st_request), 0) < 0)
        {
            perror("Failed to send ACK to client");
        }
        else
        {
            printf("ACK sent to client.\n");
        }
    }
    printf("Full path: %s\n", full_path);

    file_fd = open(full_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0)
    {
        perror("Failed to open file for writing");
        st_response error;
        error.response_type = FILE_WRITE_ERROR;
        if (send(req->socket, &error, sizeof(st_response), 0) < 0)
        {
            perror("Failed to send error response to client");
        }
        else
        {
            printf("Error response sent to client\n");
        }
        return -1;
    }

    printf("Writing to file: %s\n", req->src_path);

    // Write the data from st_request to the file
    bytes_written = write(file_fd, req->data, data_len);
    if (bytes_written < 0)
    {
        perror("Failed to write to file");
        close(file_fd);
        st_response error;
        error.response_type = FILE_WRITE_ERROR;
        if (send(req->socket, &error, sizeof(st_response), 0) < 0)
        {
            perror("Failed to send error response to client");
        }
        else
        {
            printf("Error response sent to client\n");
        }
        return -1;
    }
    // Ensure all the data has been written
    if ((size_t)bytes_written < data_len)
    {
        printf("Warning: Only %zd out of %zu bytes were written.\n", bytes_written, data_len);
    }
    else
    {
        printf("File written successfully: %s\n", req->src_path);
    }

    // Close the file descriptor
    close(file_fd);

    // Send acknowledgment to the client
    st_request ack;
    ack.request_type = ACK_MSG;
    strcpy(ack.data,"File written successfully");
    // const char *ack_msg = "ACK";
    if (send(req->socket, &ack, sizeof(st_request), 0) < 0)
    {
        perror("Failed to send ACK to client");
    }
    else
    {
        printf("ACK sent to client.\n");
    }
    return 0;
}