#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#include "../Common/network_config.h"
#include "../Common/loggers.h"
#include "nm_handler.h"
#include "../Common/requests.h"
#include "../Common/responses.h"

typedef struct NMHandlerArguments
{
    int socket;
    struct sockaddr_in nm_address;
    socklen_t nm_address_size;
} NMHandlerArguments;

void *nm_handler(void *arguments)
{
    NMHandlerArguments *nm_arguments = (NMHandlerArguments *)arguments;
    Request request_buffer;
    if (receive_request(nm_arguments->socket, &request_buffer) == -1)
    {
        log_errno_error("Error while receiving request: %s\n");
        return NULL;
    }

    switch (request_buffer.request_type)
    {
    case CREATE_REQUEST:
        log_info("CREATE_REQUEST", &nm_arguments->nm_address);

        break;
    default:
        log_info("INVALID_REQUEST_RESPONSE", &nm_arguments->nm_address);
        send_response(nm_arguments->socket, INVALID_REQUEST_RESPONSE);
        break;
    }

    free(nm_arguments);
    return NULL;
}

void *nm_connection_acceptor(void *arguments)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        log_errno_error("Error while creating Client Handler Socket: %s\n");
        return NULL;
    }
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(NM_CLIENT_HANDLER_SERVER_PORT),
        .sin_addr = {
            .s_addr = inet_addr(NM_CLIENT_HANDLER_SERVER_IP),
        }};
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        log_errno_error("Error while binding Client Handler Socket: %s\n");
        return NULL;
    }

    if (listen(server_socket, NM_CLIENT_HANDLER_TCP_WAIT_QUEUE_LENGTH) == -1)
    {
        log_errno_error("Error while listening using Client Handler Socket: %s\n");
        return NULL;
    }

    log_info("Listening for Client Connections", &server_address);

    while (1)
    {
        NMHandlerArguments *nm_handler_arguments = malloc(sizeof(NMHandlerArguments));
        if (nm_handler_arguments == NULL)
        {
            log_errno_error("Couldn't malloc: %s\n");
            return NULL;
        }
        nm_handler_arguments->socket = accept(server_socket,
                                              (struct sockaddr *)&nm_handler_arguments->nm_address,
                                              &nm_handler_arguments->nm_address_size);
        if (nm_handler_arguments->socket == -1)
        {
            log_errno_error("Error while accepting connection: %s\n");
            continue;
        }
        pthread_t client_handler_thread_id;
        pthread_create(&client_handler_thread_id, NULL, nm_handler, nm_handler_arguments);
    }
}