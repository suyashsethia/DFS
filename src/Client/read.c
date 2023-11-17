#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "read.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"


void read()
{
    char path[MAX_PATH_LENGTH + 1];
    printf("Enter Path (to read file/folder):");
    if (fgets(path, sizeof(path), stdin) == NULL) {
        return;
    }


    struct sockaddr_in nm_address = {
        .sin_family=AF_INET,
        .sin_port=htons(NM_CLIENT_HANDLER_SERVER_PORT),
        .sin_addr={
            .s_addr=inet_addr(NM_CLIENT_HANDLER_SERVER_IP),
        }
    };
    int connection_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        log_errno_error("Couldn't create socket: %s\n");
        return;
    }

    if (connect(connection_socket, (struct sockaddr *) &nm_address, sizeof(nm_address)) == -1) {
        log_errno_error("Couldn't connect to ss: %s\n");
        return;
    }
    // SENDING READ REQUEST WITH THE PATH 
    if (send_read_request(connection_socket, path) == -1) {
        log_errno_error("Couldn't send read request: %s\n");
        return;
    }
    // RECEIVING RESPONSE WITH HAS AN ADDRESS  
    char response;
    char address[100];
    if (receive_read_response(connection_socket, &response, &address) == -1) {
        log_errno_error("Couldn't receive response: %s\n");
        return;
    }
    log_response(response, &nm_address);
    close(connection_socket);

    // MAKE A NEW CONNECTION TO SS
    int ss_connection;
    // SENDING READ REQUEST WITH THE PATH 
    if (send_read_request(connection_socket, path) == -1) {
        log_errno_error("Couldn't send read request to ss: %s\n");
        return;
    }
    // RECEIVING RESPONSE WITH HAS AN ADDRESS  
    char response;
    char data[1000];
    if (receive_read_response(connection_socket, &response, &data) == -1) {
        log_errno_error("Couldn't receive response to ss: %s\n");
        return;
    }
    // printing the data
    printf("%s\n", data);
    log_response(response,&address );
    close(ss_connection);
}