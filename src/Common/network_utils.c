#include <arpa/inet.h>

#include "network_utils.h"

int send_message(int socket, Message message)
{
    if (send(socket, &message.type, sizeof(message.type), 0) == -1)
        return -1;

    if (send(socket, &message.payload_size, sizeof(message.payload_size), 0) == -1)
        return -1;

    if (message.payload_size != 0)
        if (send(socket, message.payload, message.payload_size, 0) == -1)
            return -1;

    return 0;
}

int receive_message(int socket, Message *message_buffer)
{
    if (recv(socket, &(message_buffer->type), sizeof(message_buffer->type), 0) == -1)
        return -1;

    if (recv(socket, &(message_buffer->payload_size), sizeof(message_buffer->payload_size), 0) == -1)
        return -1;

    if (recv(socket, message_buffer->payload, message_buffer->payload_size, 0) == -1)
        return -1;

    return 0;
}