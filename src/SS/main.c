#include <stdio.h>
#include <pthread.h>

#include "initialize.h"
#include "nm_handler.h"
#include "client_handler.h"

int main()
{
    initialize();

    pthread_t nm_acceptor_thread_id, client_acceptor_thread_id;
    pthread_create(&nm_acceptor_thread_id, NULL, nm_connection_acceptor, NULL);         // TODO pass ssid
    pthread_create(&client_acceptor_thread_id, NULL, client_connection_acceptor, NULL); // TODO pass ssid

    pthread_join(nm_acceptor_thread_id, NULL);
    pthread_join(client_acceptor_thread_id, NULL);
}