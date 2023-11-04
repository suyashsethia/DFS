CC = gcc
CFLAGS = -g -Wall

all: nm.out

clean:
	rm -r *.o *.out

nm.out: NM/main.o NM/client_handler.o NM/ss_handler.o Common/requests.o Common/responses.o Common/network_utils.o Common/loggers.o NM/request_handlers/create_request_handler.o
	$(CC) $(CFLAGS) $^ -o $@

NM/main.o: NM/main.c NM/client_handler.h NM/ss_handler.h
	$(CC) $(CFLAGS) -c NM/main.c -o $@

NM/client_handler.o: NM/client_handler.c NM/client_handler.h Common/requests.h Common/responses.h Common/loggers.h Common/network_config.h NM/request_handlers/create_request_handler.h
	$(CC) $(CFLAGS) -c NM/client_handler.c -o $@

NM/ss_handler.o: NM/ss_handler.c NM/ss_handler.h Common/requests.h Common/responses.h
	$(CC) $(CFLAGS) -c NM/ss_handler.c -o $@

NM/request_handlers/create_request_handler.o: NM/request_handlers/create_request_handler.h NM/request_handlers/create_request_handler.c Common/responses.h
	$(CC) $(CFLAGS) -c NM/request_handlers/create_request_handler.c -o $@

Common/requests.o: Common/requests.c Common/requests.h Common/network_utils.h 
	$(CC) $(CFLAGS) -c Common/requests.c -o $@

Common/responses.o: Common/responses.c Common/responses.h Common/network_utils.h
	$(CC) $(CFLAGS) -c Common/responses.c -o $@

Common/network_utils.o: Common/network_utils.c Common/network_utils.h
	$(CC) $(CFLAGS) -c Common/network_utils.c -o $@

Common/loggers.o: Common/loggers.c Common/loggers.h Common/colors.h
	$(CC) $(CFLAGS) -c Common/loggers.c -o $@