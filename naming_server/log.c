#include "headers.h"

void log_message(char *message, char *color){
    printf("%s%s%s\n", color, message, RESET_COLOR);
}