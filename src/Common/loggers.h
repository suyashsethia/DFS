#ifndef LOGGERS_H
#define LOGGERS_H

void log_error(const char *error);
void log_errno_error(const char *error_format);
void log_info(const char *message, struct sockaddr_in *client_address);

#endif // LOGGERS_H