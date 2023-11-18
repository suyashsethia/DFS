#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "write.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"

void write()
{
    char path[MAX_PATH_LENGTH + 1];
    printf("Enter Path (to write file):");
    if (fgets(path, sizeof(path), stdin) == NULL)
    {
        return;
    }

    size_t path_size = strlen(path);
    path[path_size - 1] = '\0'; // set newline to null
    printf("Enter the data you want to write :\n");
    char data[1024];
    if (fgets(data, sizeof(data), stdin) == NULL)
        return;

    struct sockaddr_in nm_address = {
        .sin_family = AF_INET,
        .sin_port = htons(NM_CLIENT_HANDLER_SERVER_PORT),
        .sin_addr = {
            .s_addr = inet_addr(NM_CLIENT_HANDLER_SERVER_IP),
        }};
    int connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection_socket == -1)
    {
        log_errno_error("Couldn't create socket: %s\n");
        return;
    }

    if (connect(connection_socket, (struct sockaddr *)&nm_address, sizeof(nm_address)) == -1)
    {
        log_errno_error("Couldn't connect to nm: %s\n");
        return;
    }
    // SENDING READ REQUEST WITH THE PATH
    if (send_write_request(connection_socket, path) == -1)
    {
        log_errno_error("Couldn't send write request: %s\n");
        return;
    }
    // printf("Sent write request\n");
    // RECEIVING RESPONSE WITH HAS AN ADDRESS
    char response;
    // char address[100];
    struct sockaddr_in address_ss;

    if (receive_response(connection_socket, &response) == -1)
    {
        log_errno_error("Couldn't receive response: %s\n");
        return;
    }
    printf("Received response %c\n ", response);
    if (response == REDIRECT_RESPONSE)
    {
        if (receive_redirect_response_payload(connection_socket, &address_ss) == -1)
        {
            log_errno_error("Couldn't receive address: %s\n");
            return;
        }

        log_response(response, &nm_address);
        close(connection_socket);
        // printf("ddd");
        // MAKE A NEW CONNECTION TO SS
        // int ss_connection;
        int ss_connection = socket(AF_INET, SOCK_STREAM, 0);
        if (ss_connection == -1)
        {
            log_errno_error("Couldn't create socket: %s\n");
            return;
        }

        if (connect(connection_socket, (struct sockaddr *)&address_ss, sizeof(address_ss)) == -1)
        {
            log_errno_error("Couldn't connect to ss: %s\n");
            return;
        }

        // SENDING WRITE REQUEST WITH THE PATH
        if (send_write_request(ss_connection, path) == -1)
        {
            log_errno_error("Couldn't send write request: %s\n");
            return;
        }

        char response;
        if (receive_response(ss_connection, &response) == -1)
        {
            log_errno_error("Couldn't receive response: %s\n");
            return;
        }
        // RECEIVING RESPONSE WITH HAS AN ADDRESS

        if (response == OK_START_STREAM_RESPONSE)
        {
            while (1)
            {
                char data[MAX_STREAMING_RESPONSE_PAYLOAD_SIZE];
                if (fgets(data, sizeof(data), stdin) == NULL)
                    return;
                if (strcmp(data, "END"))
                {
                    end_streaming_response_payload(ss_connection);
                    log_response(response, &address_ss);
                    close(ss_connection);
                    return;
                }
                int r = send_streaming_response_payload(ss_connection, data, sizeof(data));
                if (r == -1)
                {
                    log_errno_error("Couldn't send data: %s\n");
                    return;
                }
            }
            printf("Data sending over\n");
        }
    }
    log_response(response, &nm_address);
    close(connection_socket);
}