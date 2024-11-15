#include "functions.h"

void write_file(st_request* req) {
    int file_fd;
    ssize_t bytes_written;
    size_t data_len = strlen(req->data);

    // Open the file for writing (create if not exists, truncate if exists)
    file_fd = open(req->path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {  
        perror("Failed to open file for writing");
        const char *error_msg = "Error: Unable to open the file for writing.\n";
        send(req->socket, error_msg, strlen(error_msg), 0);
        return;
    }

    printf("Writing to file: %s\n", req->path);

    // Write the data from st_request to the file
    bytes_written = write(file_fd, req->data, data_len);
    if (bytes_written < 0) {
        perror("Failed to write to file");
        close(file_fd);
        return;
    }
    // Ensure all the data has been written
    if ((size_t)bytes_written < data_len) {
        printf("Warning: Only %zd out of %zu bytes were written.\n", bytes_written, data_len);
    } else {
        printf("File written successfully: %s\n", req->path);
    }

    // Close the file descriptor
    close(file_fd);

    // Send acknowledgment to the client
    st_request ack;
    ack.request_type=ACK_MSG;
    // const char *ack_msg = "ACK";
    if (send(req->socket, &ack, sizeof(st_request), 0) < 0) {
        perror("Failed to send ACK to client");
    } else {
        printf("ACK sent to client.\n");
    }
}