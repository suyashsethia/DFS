#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>

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

    strncpy(request.request_content.create_request_data.path, file_path, sizeof(request.request_content.create_request_data.path));

    return send_request(socket, &request);
}

int send_create_backup_request(int socket, const char *file_path, bool is_folder)
{
    Request request = {.request_type = CREATE_BACKUP_REQUEST,
                       .request_content = {.create_backup_request_data = {.is_folder = is_folder}}};

    strncpy(request.request_content.create_backup_request_data.path, file_path, sizeof(request.request_content.create_backup_request_data.path));

    return send_request(socket, &request);
}

int receive_request(int socket, Request *request_buffer)
{
    Message message = {.payload = request_buffer};
    return receive_message(socket, &message);
}