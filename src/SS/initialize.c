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
        perror("Unable to open directory");
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
            fprintf(stderr, "Error: Memory allocation failed\n");
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

    // connecting NM
    int server_port = SS_NM_HANDLER_BASE_PORT + atoi(ss_id);


    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Error while creating NM Handler Socket:\n");
        return 0;
    }

    struct sockaddr_in ss_nm_server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(server_port),
        .sin_addr = {
            .s_addr = inet_addr(SS_NM_HANDLER_IP),
        }};


    // connecting SS to Client

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    int client_port = SS_CLIENT_HANDLER_BASE_PORT + atoi(ss_id);
    if (client_socket == -1)
    {
        printf("Error while creating Client Handler Socket:\n");
        return 0;
    }
    struct sockaddr_in SS_client_address = {
        .sin_family = AF_INET,
        .sin_port = htons(client_port),
        .sin_addr = {
            .s_addr = inet_addr(SS_CLIENT_HANDLER_IP),
        }};

    send_register_ss_request(server_socket, atoi(ss_id), &ss_nm_server_address, &SS_client_address, paths_count, list_of_paths);

    return 0;
}