#include "headers.h"

// Function to log messages to file
void logMessage(const char *operation, const char *ip, int port, int clientID, const char *details, const char *status) {
    LogEntry log;
    log.timestamp = time(NULL);  // Set the current time
    // strncpy(log.operationType, operation, sizeof(log.operationType) - 1);
    strncpy(log.clientIP, ip, sizeof(log.clientIP) - 1);
    log.clientPort = port;
    log.clientID = clientID;
    strncpy(log.requestDetails, details, sizeof(log.requestDetails) - 1);
    strncpy(log.ackStatus, status, sizeof(log.ackStatus) - 1);

    // Open the log file in append mode
    FILE *file = fopen("log.txt", "a");
    if (file == NULL) {
        perror("Failed to open log file");
        return;
    }

    // Write the log entry to the file
    // fprintf(file, "[%s] Operation: %s | Client ID: %d | IP: %s:%d | Status: %s | Details: %s\n", ctime(&log.timestamp), log.operationType, log.clientID, log.clientIP, log.clientPort, log.ackStatus, log.requestDetails);

    // Close the file
    fclose(file);
}