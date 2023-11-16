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

int initialize()
{
    //send request for connection , initialization to nm 

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Error while creating Client Handler Socket: %s\n");
        return NULL;
    }
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(NM_SS_HANDLER_SERVER_PORT),
        .sin_addr = {
            .s_addr = inet_addr(NM_SS_HANDLER_SERVER_IP),
        }};
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        printf("Error while binding SS Handler Socket: %s\n");
        return NULL;
    }

    
}