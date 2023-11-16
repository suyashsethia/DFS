#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#include "client_handler.h"
#include "../Common/loggers.h"
#include "../Common/network_config.h"

typedef struct ClientHandlerArguments
{
    int socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size;
} ClientHandlerArguments;

void *client_handler(void *arguments)
{
}

void *client_connection_acceptor(void *arguments)
{
    int ssid = ((ClientConnectionAcceptorArguments *)arguments)->ssid;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        log_errno_error("Error while creating client handler socket: %s\n");
        return NULL;
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(SS_CLIENT_HANDLER_BASE_PORT + ssid),
        .sin_addr = {
            .s_addr = inet_addr(SS_CLIENT_HANDLER_IP)}};

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        log_errno_error("Error while binding Client Handler Socket: %s\n");
        return NULL;
    }

    if (listen(server_socket, SS_CLIENT_HANDLER_TCP_WAIT_QUEUE) == -1)
    {
        log_errno_error("Error while listening using Client Handler Socket: %s\n");
        return NULL;
    }

    log_info("Listening for Client Connections", &server_address);
    while (1)
    {
        ClientHandlerArguments *client_handler_arguments = malloc(sizeof(ClientHandlerArguments));
        if (client_handler_arguments == NULL)
        {
            log_errno_error("Couldn't malloc: %s\n");
            return NULL;
        }
        client_handler_arguments->socket = accept(server_socket,
                                                  (struct sockaddr *)&client_handler_arguments->client_address,
                                                  &client_handler_arguments->client_address_size);
        if (client_handler_arguments->socket == -1)
        {
            log_errno_error("Error while accepting connection: %s\n");
            continue;
        }
        pthread_t client_handler_thread_id;
        pthread_create(&client_handler_thread_id, NULL, client_handler, client_handler_arguments);
    }
}