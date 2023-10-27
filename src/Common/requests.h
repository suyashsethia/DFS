#ifndef REQUESTS_H
#define REQUESTS_H

#define MAX_PATH_LENGTH 4096UL

#define MESSAGE_TYPE_REQUEST '0'

#define CREATE_REQUEST '0'
#define CREATE_BACKUP_REQUEST '1'

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

union RequestContent
{
    CreateRequestData create_request_data;
    CreateBackupRequestData create_backup_request_data;
};

typedef struct Request
{
    char request_type;
    union RequestContent request_content;
} Request;

int send_create_request(int socket, const char *file_path, bool is_folder);
int send_create_backup_request(int socket, const char *file_path, bool is_folder);
int receive_request(Request *request_buffer);

#endif // REQUESTS_H