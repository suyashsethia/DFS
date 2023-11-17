#include <stdio.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "copy.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"


void copy()
{
    char source_path[MAX_PATH_LENGTH + 1];
    printf("Enter Source path :\n");
    if (fgets(source_path, sizeof(source_path), stdin) == NULL) {
        return;
    }
    size_t path_size = strlen(source_path);
    source_path[path_size - 1] = '\0'; // set newline to null

    char destination_path[MAX_PATH_LENGTH + 1];
    printf("Enter Destination path :\n");
    if (fgets(destination_path, sizeof(destination_path), stdin) == NULL) {
       return;
    }
    size_t path_size = strlen(destination_path);
    destination_path[path_size - 1] = '\0'; // set newline to null

    printf("Copy a File(F) or a Directory(D):");
    char file_or_folder[1024];
    if (fgets(file_or_folder, sizeof(file_or_folder), stdin) == NULL)
        return;
    bool is_folder;
    if (file_or_folder[0] == 'F') {
        is_folder = false;
    } else if (file_or_folder[0] == 'D') {
        is_folder = true;
    } else {
        printf("Invalid Option\n");
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
    if (send_copy_request(connection_socket, source_path, destination_path, is_folder) == -1) {
        log_errno_error("Couldn't send copy request: %s\n");
        return;
    }
    // RECEIVING RESPONSE WITH HAS AN ADDRESS  
    char response;
    if (receive_copy_response(connection_socket, &response) == -1) {
        log_errno_error("Couldn't copy response: %s\n");
        return;
    }
    log_response(response, &nm_address);
    close(connection_socket);
}