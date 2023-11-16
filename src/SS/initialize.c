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

#include "initialize.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"
#include "../Common/loggers.h"

int create_folder(const char *path)
{
    return mkdir(path, 0777);
}

int if_folder_exhists(char *ss_id)
{
    // check if folder named ss_id exists in the current directory
    int x = opendir(ss_id) == NULL ? 0 : 1;
    // printf("x: %d\n", x);
    return x;
}
void recursive_path_finder(char *ss_id, char *list_of_paths[100], int *paths_count)
{
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(ss_id);
    if (dir == NULL)
    {
        log_errno_error("Unable to open directory");
        // perror("Unable to open directory");
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignore "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Construct the full path
        char path[MAX_PATH_LENGTH]; // Adjust the size as needed
        snprintf(path, sizeof(path), "%s/%s", ss_id, entry->d_name);
        // Check if the entry is a directory
        if (entry->d_type == NULL || entry->d_type != DT_DIR)
        {
            break;
        }
        else if (entry->d_type == DT_DIR)
        {
            // Recursively call the function for subdirectories
            strcpy(list_of_paths[(*paths_count)++], path);

            recursive_path_finder(path, list_of_paths, paths_count);
        }
    }

    // Close the directory
    closedir(dir);
}

int initialize(int argc, char *argv[])
{
    char *list_of_paths[MAX_ACCESIBLE_PATHS];
    for (int i = 0; i < 100; ++i)
    {
        list_of_paths[i] = (char *)malloc(sizeof(char)); // Adjust the size as needed
        if (list_of_paths[i] == NULL)
        {
            log_errno_error("Error: Memory allocation failed");
            // fprintf(stderr, "Error: Memory allocation failed\n");
            return 1;
        }
    }

    char ss_id[MAX_SS_COUNT];
    strcpy(ss_id, argv[1]);
    printf("ss_id: %s\n", ss_id);

    int flag = if_folder_exhists(ss_id);
    int paths_count = 0;
    if (flag == 0)
    {
        char path[MAX_PATH_LENGTH]; // Adjust the size as needed
        strcpy(path, ss_id);        // Copy the original string to the destination
        strcat(path, "/");
        create_folder(ss_id);
        strcpy(list_of_paths[(paths_count)++], path);
    }
    else
    {
        recursive_path_finder(ss_id, list_of_paths, &paths_count);
    }

    int nm_init_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (nm_init_socket == -1)
    {
        log_errno_error("Error while creating NM Init Socket:\n");
        // printf("Error while creating NM Init Socket:\n");
        return 0;
    }

    struct sockaddr_in nm_init_server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(NM_SS_HANDLER_SERVER_PORT),
        .sin_addr = {
            .s_addr = inet_addr(NM_SS_HANDLER_SERVER_IP),
        }};

    int k = connect(nm_init_socket, (struct sockaddr *)&nm_init_server_address, sizeof(nm_init_server_address));
    if (k == -1)
    {
        log_errno_error("Error while connecting NM Init Socket:\n");
    }

    // connecting NM
    int server_port = SS_NM_HANDLER_BASE_PORT + atoi(ss_id);

    struct sockaddr_in ss_nm_server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(server_port),
        .sin_addr = {
            .s_addr = inet_addr(SS_NM_HANDLER_IP),
        }};

    // connecting SS to Client

    // int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    int client_port = SS_CLIENT_HANDLER_BASE_PORT + atoi(ss_id);
    struct sockaddr_in SS_client_address = {
        .sin_family = AF_INET,
        .sin_port = htons(client_port),
        .sin_addr = {
            .s_addr = inet_addr(SS_CLIENT_HANDLER_IP),
        }};

    if (send_register_ss_request(nm_init_socket, atoi(ss_id), &ss_nm_server_address, &SS_client_address, paths_count, list_of_paths) == 0)
    {
        log_info("Initialised SS connection with NM\n" , nm_init_socket);
    }
    else
    {
        log_errno_error("Error while sending register request to NM\n");
    }
    return 0;
}