#ifndef RESPONSES_H
#define RESPONSES_H

#define NOT_FOUND_RESPONSE '0'
#define INTERNAL_ERROR_RESPONSE '1'

#define OK_RESPONSE '2'

int send_response(int socket, char response_type);
int receive_response(int socket, char *response_buffer);

#endif // RESPONSES_H