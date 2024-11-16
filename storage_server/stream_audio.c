#include "functions.h"
#define SEND_DELAY_USEC 10000
void stream_audio(st_request *req)
{
    printf("Streaming audio at handler function\n");
    int client_sock=req->socket;
    FILE *file = fopen(req->path, "rb");
    if (file == NULL)
    {
        perror("File not found or cannot be opened");
        close(client_sock);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        if (send(client_sock, buffer, bytes_read, 0) < 0)
        {
            perror("Failed to send file data");
            break;
        }
        usleep(SEND_DELAY_USEC); // Add a delay between sends
    }

    fclose(file);
    close(client_sock);
}