#ifndef CREATE_REQUEST_HANDLER_H
#define CREATE_REQUEST_HANDLER_H

char create_request_handler(int client_socket, struct sockaddr_in client_socket_address, char *path, bool is_folder);

#endif // CREATE_REQUEST_HANDLER_H