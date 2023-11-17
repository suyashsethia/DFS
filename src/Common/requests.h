#ifndef REQUESTS_H
#define REQUESTS_H

#define MAX_PATH_LENGTH 4096UL
#define MAX_ACCESIBLE_PATHS 400

#define MESSAGE_TYPE_REQUEST '0'
#define MESSAGE_TYPE_SS_REGISTER_PAYLOAD 'S'

#define CREATE_REQUEST '0'
#define CREATE_BACKUP_REQUEST '1'
#define SS_REGISTER_REQUEST '2'
#define DELETE_REQUEST '3'

#include <stdbool.h>
#include <stdint.h>
#include <arpa/inet.h>

typedef struct CreateRequestData
{
    bool is_folder;
    char path[MAX_PATH_LENGTH];
} CreateRequestData;

typedef struct CreateBackupRequestData
{
    bool is_folder;
    char path[MAX_PATH_LENGTH];
} CreateBackupRequestData;

typedef struct DeleteRequestData
{
    char path[MAX_PATH_LENGTH];
} DeleteRequestData;

typedef struct SSRegisterData
{
    int ss_id;
    struct sockaddr_in nm_connection_address;
    struct sockaddr_in client_connection_address;
    uint64_t accessible_paths_count;
    char **accessible_paths;
} SSRegisterData;

union RequestContent
{
    CreateRequestData create_request_data;
    CreateBackupRequestData create_backup_request_data;
    SSRegisterData ss_register_data;
    DeleteRequestData delete_request_data;
};

typedef struct Request
{
    char request_type;
    union RequestContent request_content;
} Request;

int send_create_request(int socket, const char *file_path, bool is_folder);

int send_create_backup_request(int socket, const char *file_path, bool is_folder);

int send_register_ss_request(int socket, int ss_id, struct sockaddr_in *nm_connection_address,
                             struct sockaddr_in *client_connection_address,
                             uint64_t accessible_paths_count,
                             char accessible_paths[accessible_paths_count][MAX_PATH_LENGTH]);

int send_delete_request(int socket, const char *path);

int receive_request(int socket, Request *request_buffer);

#endif // REQUESTS_H