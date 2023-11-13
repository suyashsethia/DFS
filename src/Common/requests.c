#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "network_utils.h"
#include "requests.h"

int send_request(int socket, Request *request)
{

    Message message = {.type = MESSAGE_TYPE_REQUEST,
        .payload_size = sizeof(*request),
        .payload = request};
    return send_message(socket, message);
}

int send_create_request(int socket, const char *file_path, bool is_folder)
{

    Request request = {.request_type = CREATE_REQUEST,
        .request_content = {.create_request_data = {.is_folder = is_folder}}};

    strncpy(request.request_content.create_request_data.path, file_path,
            sizeof(request.request_content.create_request_data.path));

    return send_request(socket, &request);
}

int send_create_backup_request(int socket, const char *file_path, bool is_folder)
{
    Request request = {.request_type = CREATE_BACKUP_REQUEST,
        .request_content = {.create_backup_request_data = {.is_folder = is_folder}}};

    strncpy(request.request_content.create_backup_request_data.path, file_path,
            sizeof(request.request_content.create_backup_request_data.path));

    return send_request(socket, &request);
}

int send_register_ss_request(int socket, int ss_id, struct sockaddr_in *nm_connection_address,
                             struct sockaddr_in *client_connection_address,
                             uint64_t accessible_paths_count,
                             char accessible_paths[accessible_paths_count][MAX_PATH_LENGTH])
{
    Request request = {
        .request_type=SS_REGISTER_REQUEST,
        .request_content={.ss_register_data={
            .ss_id=ss_id,
            .nm_connection_address=*nm_connection_address,
            .client_connection_address=*client_connection_address,
            .accessible_paths_count=accessible_paths_count,
            .accessible_paths=NULL
        }}
    };
    if (send_request(socket, &request) == -1)
        return -1;
    Message accessible_paths_payload = {
        .type=MESSAGE_TYPE_SS_REGISTER_PAYLOAD,
        .payload_size=accessible_paths_count * MAX_PATH_LENGTH,
        .payload=accessible_paths};

    return send_message(socket, accessible_paths_payload);

}

int receive_ss_request_payload(int socket, Message *message_buffer, uint64_t path_count)
{
    char accessible_paths_buffer[path_count][MAX_PATH_LENGTH];
    message_buffer->payload = accessible_paths_buffer;
    if (receive_message(socket, message_buffer) == -1) return -1;
    char **accessible_paths = malloc(sizeof(char *) * path_count);
    for (uint64_t i = 0; i < path_count; ++i) {
        uint64_t path_length = strlen(accessible_paths_buffer[i]);
        char *path = malloc(path_length + 1); // there's always space for jesus ('\0')
        strcpy(path, accessible_paths_buffer[i]);
        accessible_paths[i] = path;
    }
    message_buffer->payload = accessible_paths;
    return 0;
}

int receive_request(int socket, Request *request_buffer)
{
    Message message = {.payload = request_buffer};
    if (receive_message(socket, &message) == -1) return -1;
    Message message_buffer;
    switch (request_buffer->request_type) {
        case SS_REGISTER_REQUEST:
            if (receive_ss_request_payload(socket, &message_buffer,
                                           request_buffer->request_content.ss_register_data.accessible_paths_count) ==
                -1)
                return -1;
            request_buffer->request_content.ss_register_data.accessible_paths = message_buffer.payload;
            break;
        default:
            break;
    }
    return 0;
}