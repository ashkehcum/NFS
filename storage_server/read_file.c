#include "functions.h"

void read_file(st_request *req)
{   
    printf("Reading file: %s\n", req->path);
    printf("Reading file data: %s\n", req->data);
    char buffer[CHUNK_SIZE];
    int file_fd;
    ssize_t bytes_read, bytes_sent;

    // Open the file for reading
    file_fd = open(req->path, O_RDONLY);
    if (file_fd < 0)
    {
        perror("Failed to open file");
        const char *error_msg = "Error: Unable to open the requested file.\n";
        send(req->socket, error_msg, strlen(error_msg), 0);
        return;
    }
    st_request read_request;
    // Read the file and send its content in chunks to the client
    while ((bytes_read = read(file_fd, read_request.data, CHUNK_SIZE)) > 0)
    {
        printf("Bytes read: %zd\n", bytes_read);
        printf("Data read: %s\n", read_request.data);

        bytes_sent = send(req->socket, &read_request, sizeof(st_request), 0);
        if (bytes_sent < 0)
        {
            perror("Failed to send data to client");
            close(file_fd);
            return;
        }
    }

    if (bytes_read < 0)
    {
        perror("Failed to read from file");
        const char *error_msg = "Error: Unable to read the requested file.\n";
        send(req->socket, error_msg, strlen(error_msg), 0);
    }
    else
    {
        const char *success_msg = "File sent successfully.\n";
        printf("File sent successfully.\n");
        st_request ack;
        // ack.socket
        ack.request_type=ACK_MSG;
        send(req->socket, &ack, sizeof(st_request), 0);
    }

    // Close the file descriptor
    close(file_fd);
}