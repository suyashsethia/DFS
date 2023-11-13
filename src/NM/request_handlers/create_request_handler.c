#include <stdbool.h>
#include <arpa/inet.h>

#include "../../Common/responses.h"
#include "../../Common/loggers.h"

char create_request_handler(int client_socket, struct sockaddr_in client_socket_address, char *path, bool is_folder)
{
    if (*path == '\0' || *path == '.' || *path == '/') {
        log_info("INVALID_REQUEST_CONTENT_RESPONSE", &client_socket_address);
        return INVALID_REQUEST_CONTENT_RESPONSE;
    }

    return NOT_FOUND_RESPONSE;
}