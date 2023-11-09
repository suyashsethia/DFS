#include "ss.h"
#include "network_utils.h"
#include "requests.h"
#include "responses.h"

int create_folder(const char *path)
{
    return mkdir(path, 0777);
}
int create_file(const char *path)
{
    FILE *file = fopen(path, "w");
    if (file == NULL)
    {
        return -1;
    }
    fclose(file);
    return 0;
}
int main()
{
    // Request request_buffer = (Request *)calloc(1, sizeof(Request));
    Request request_buffer;
    if (receive_request(&request_buffer) == 0)
    {
        printf("Request received\n");
        // print request with time stamp socket id and request type
        if (request_buffer.request_type == CREATE_REQUEST)
        {
            // printf("Create request received\n");
            if (request_buffer.request_content.create_request_data.is_folder)
            {
                // printf("Folder creation request received\n");
                // printf("Folder path: %s\n", request_buffer.request_content.create_request_data.path);
                if (create_folder(request_buffer.request_content.create_request_data.path))
                {
                    time_t current_time;
                    time(&current_time);
                    printf("Request type: %c\n , Time:%s\n , Socket_id:%s\n", request_buffer.request_type, ctime(&current_time), 0);
                    send_response(socket, OK_RESPONSE);
                }
                else
                {
                    send_response(socket, INTERNAL_ERROR_RESPONSE);
                }
            }
            else
            {
                // printf("File creation request received\n");
                // printf("File path: %s\n", request_buffer.request_content.create_request_data.path);
                if (create_file(request_buffer.request_content.create_request_data.path))
                {
                    time_t current_time;
                    time(&current_time);
                    printf("Request type: %c\n , Time:%s\n , Socket_id:%s\n", request_buffer.request_type, ctime(&current_time), 0);
                    send_response(socket, OK_RESPONSE);
                }
                else
                {
                    send_response(socket, INTERNAL_ERROR_RESPONSE);
                }
            }
        }
        // else if(request_buffer.request_type==CREATE_BACKUP_REQUEST)
        // {
        //     printf("Create backup request received\n");
        //     if(request_buffer.request_content.create_backup_request_data.is_folder)
        //     {
        //         printf("Folder backup request received\n");
        //         printf("Folder path: %s\n", request_buffer.request_content.create_backup_request_data.path);
        //         send_response(socket, '0');
        //     }
        //     else
        //     {
        //         printf("File backup request received\n");
        //         printf("File path: %s\n", request_buffer.request_content.create_backup_request_data.path);
        //         send_response(socket, '0');
        //     }
        // }
        // else
        // {
        //     printf("Invalid request type\n");
        //     send_response(socket, '404');
        // }
    }
    else
    {
        printf("Error in receiving request\n");
        send_response(socket, INTERNAL_ERROR_RESPONSE);
    }
}