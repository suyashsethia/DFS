#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>

#include "initialize.h"
#include "../Common/network_config.h"
#include "../Common/requests.h"
#include "../Common/responses.h"

#include "create.h"

// int if_folder_exhists(char *ss_id)
// {
//     // check if folder named ss_id exists in the current directory
//     if (opendir(ss_id) == NULL)
//     {
//         // create folder named ss_id
//         if (create_folder(ss_id))
//         {
//             printf("Folder created\n");
//             return 1;
//         }
//         else
//         {
            
//             printf("Folder creation failed\n");
//         }
//     }
//     else
//     {
//         return 0 ; 
//     }
// }


int initialize(int argc, char *argv[])
{
    //send request for connection , initialization to nm

    // take command line arguments for ss_id 

    char ss_id[100] ;
    strcpy(ss_id, argv[1]);
    printf("ss_id: %s\n", ss_id);

    // check if folder named ss_id exists in the current directory
    // if(if_folder_exhists(ss_id))
    // {

    // }
    // else
    // {

    // }

    // int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // if (server_socket == -1) {
    //     printf("Error while creating Client Handler Socket: %s\n");
    //     return NULL;
    // }
    // struct sockaddr_in server_address = {
    //     .sin_family = AF_INET,
    //     .sin_port = htons(NM_SS_HANDLER_SERVER_PORT),
    //     .sin_addr = {
    //         .s_addr = inet_addr(NM_SS_HANDLER_SERVER_IP),
    //     }};
    // if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
    //     printf("Error while binding SS Handler Socket: %s\n");
    //     return NULL;
    // }

    return 0;
}