#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../Common/requests.h"
#include "../Common/responses.h"

void create(int client_socket)
{
    printf("Enter the path of the file\n");
    char path[100];
    scanf("%s", path);
    printf("Enter the type of file (folder or file)\n");
    char type[100];
    scanf("%s", type);
    int return_value;
    if (strcmp(type, "folder"))
    {
        return_value = send_create_request(client_socket, path, true);
    }
    else
    {
        return_value = send_create_request(client_socket, path, false);
    }
    if( return_value){
        printf("Request Sent Successfully\n");
        Request request_buffer;
        if (receive_response(client_socket, &request_buffer) == 0)
        {
            printf("File created successfully\n");
        }
        else{
            printf("File not created\n");
        }
        
    }
    else{
        printf("Request failed\n");
    }
}

void create_backup(int client_socket)
{
    printf("Enter the path of the file\n");
    char path[100];
    scanf("%s", path);
    printf("Enter the type of file (folder or file)\n");
    char type[100];
    scanf("%s", type);
    int return_value;
    if (strcmp(type, 'folder'))
    {
        return_value = send_create_backup_request(client_socket, path, true);
    }
    else
    {
        return_value = send_create_backup_request(client_socket, path, false);
    }
    return return_value;
}

int main()
{

    // creation and connection of client socket to NM

    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Cannot create socket");
        return -1;
    }

    // Define the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8001);
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    { 
        perror("Invalid address/ Address not supported");
        return -1;
    }

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        return -1;
    }

    printf("Client has started\n");
    printf("Connected to the server\n");
    printf(" If you want to exit the write 'exit'\n");
    while (1)
    {
        char command[100];
        printf("ENTER THE COMMAND (all caps)\n");
        scanf("%s", command);
        if (strcmp(command, 'end'))
        {
            break;
        }
        if (strcmp(command, 'CREATE'))
        {
            create(client_socket);
        }
        else if (strcmp(command, 'CREATE_BACKUP'))
        {
            create_backup(client_socket);
        }
        // else if( strcmp(command, 'REGISTER_SS')){
        //     printf("Enter the id of the ss\n");
        //     int id;
        //     scanf("%d", &id);
        //     printf("Enter the ip address of the ss\n");
        //     char ip[100];
        //     scanf("%s", ip);
        //     printf("Enter the port number of the ss\n");
        //     int port;
        //     scanf("%d", &port);
        //     printf("Enter the number of accessible paths\n");
        //     int num;
        //     scanf("%d", &num);
        //     char paths[num][100];
        //     for(int i=0; i<num; i++){
        //         printf("Enter the path\n");
        //         scanf("%s", paths[i]);
        //     }
        //     struct sockaddr_in nm_connection_address = create_socket_address(ip, port);
        //     struct sockaddr_in client_connection_address = create_socket_address("
        // }
    }

    printf("Thank you, Client is closed, Happy diwali\n");
    close(client_socket);
}