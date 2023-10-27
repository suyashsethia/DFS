#include <arpa/inet.h>

#include "responses.h"

int send_response(int socket, char response_type)
{
    return send(socket, &response_type, 1, 0);
}

int receive_response(int socket, char *response_buffer)
{
    return recv(socket, response_buffer, 1, 0);
}