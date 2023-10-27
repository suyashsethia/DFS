#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h>

typedef struct Message
{
    char type;
    uint64_t payload_size;
    void *payload;
} Message;

int send_message(int socket, Message);
int receive_message(int socket, Message *message_buffer);

#endif // NETWORK_UTILS_H