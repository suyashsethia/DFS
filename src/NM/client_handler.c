#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>

#include "client_handler.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"
#include "../Common/network_config.h"
#include "request_handlers/create_request_handler.h"

typedef struct ClientHandlerArguments
{
    int socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size;
} ClientHandlerArguments;

void *client_handler(void *client_handler_arguments_raw)
{
    ClientHandlerArguments *client_handler_arguments = (ClientHandlerArguments *)client_handler_arguments_raw;
    Request request_buffer;
    if (receive_request(client_handler_arguments->socket, &request_buffer) == -1)
    {
        log_errno_error("Error while receiving request: %s\n");
        return NULL;
    }

    switch (request_buffer.request_type)
    {
    case CREATE_REQUEST:
        log_info("CREATE_REQUEST", &client_handler_arguments->client_address);
        char response = create_request_handler(client_handler_arguments->socket,
                                               client_handler_arguments->client_address,
                                               request_buffer.request_content.create_request_data.path,
                                               request_buffer.request_content.create_request_data.is_folder);
        log_response(response, &client_handler_arguments->client_address);
        if (send_response(client_handler_arguments->socket, response) == -1)
        {
            log_errno_error("Couldn't send response: %s\n");
        }
        break;
    default:
        log_info("INVALID_REQUEST_RESPONSE", &client_handler_arguments->client_address);
        send_response(client_handler_arguments->socket, INVALID_REQUEST_RESPONSE);
        break;
    }

    free(client_handler_arguments);
    return NULL;
}

void *client_connection_acceptor_thread()
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
