#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#include "client_handler.h"
#include "../Common/loggers.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"

typedef struct ClientHandlerArguments
{
    int socket;
    struct sockaddr_in client_address;
    socklen_t client_address_size;
} ClientHandlerArguments;

int read_file_and_send_data(const char *path, int client_socket)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        log_errno_error("Error while opening file:\n");
        return -1;
    }
    fseek(file, 0, SEEK_SET);

    char buffer[MAX_STREAMING_RESPONSE_PAYLOAD_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, MAX_STREAMING_RESPONSE_PAYLOAD_SIZE, file)) > 0)
    {
        if (send_streaming_response_payload(client_socket, buffer, bytes_read) == -1)
        {
            log_errno_error("Error while sending data to client:\n");
            return -1;
        }
    }
    end_streaming_response_payload(client_socket);
    fclose(file);
    return 0;
}

void *client_handler(void *arguments)
{
    ClientHandlerArguments *client_ss_handler_arguments = (ClientHandlerArguments *)arguments;
    Request request_buffer;
    if (receive_request(client_ss_handler_arguments->socket, &request_buffer) == -1)
    {
        log_errno_error("Error while receiving request from client:\n");
        return NULL;
    }
    char response;
    switch (request_buffer.request_type)
    {
    case READ_REQUEST:
        log_info("READ_REQUEST", &client_ss_handler_arguments->client_address);
        if (read_file_and_send_data(request_buffer.request_content.read_request_data.path, client_ss_handler_arguments->socket) == -1)
        {
            response = INTERNAL_ERROR_RESPONSE;
        }
        else
        {
            response = OK_RESPONSE;
        }
        send_response(client_ss_handler_arguments->socket, response);
        break;

    case WRITE_REQUEST:
    case COPY_REQUEST:
    default:
        log_info("Invalid Request Type", &client_ss_handler_arguments->client_address);
        response = INVALID_REQUEST_RESPONSE;
        send_response(client_ss_handler_arguments->socket, response);
        break;
    }
    free(client_ss_handler_arguments);
    return NULL;
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