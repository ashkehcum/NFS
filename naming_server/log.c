#include "headers.h"

// Function to log messages to file
void LogMessage(bool isClient, char* IP_Addr, int Port_No, st_request request, int response_code) {
    FILE* log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        printf("Error opening log file\n");
        return;
    }
    time_t current_time;
    time(&current_time);
    struct tm* time_info = localtime(&current_time);
    char time_str[30];
    strftime(time_str, 30, "%Y-%m-%d %H:%M:%S", time_info);
    if (isClient) {
        fprintf(log_file, "%s: Client %s:%d requested %d type %s %s %s %d\n", time_str, IP_Addr, Port_No, request.request_type, request.data, request.path, request.copy_to_path, response_code);
    } else {
        fprintf(log_file, "%s: Storage Server %s:%d requested %s type %s %s %s %d\n", time_str, IP_Addr, Port_No, request.request_type, request.data, request.path, request.copy_to_path, response_code);
    }
    fclose(log_file);
}