#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "loggers.h"
#include "colors.h"

void log_error(const char *error)
{
    fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET, error);
}

void log_errno_error(const char *error_format)
{
    const char *error_reason = strerror(errno);
    size_t size = snprintf(NULL, 0, error_format, error_reason);
    char error_message[size + 1];
    sprintf(error_message, error_format, error_reason);
    log_error(error_message);
}