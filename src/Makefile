CC = gcc
CFLAGS = -g -Wall

all: nm.out

clean:
	rm -r *.o *.out

nm.out: NM/main.o NM/client_handler.o NM/ss_handler.o Common/requests.o Common/responses.o Common/network_utils.o Common/loggers.o NM/request_handlers/create_request_handler.o NM/ss_info.o NM/trie.o Common/path_utils.o
	$(CC) $(CFLAGS) $^ -o $@

NM/main.o: NM/main.c NM/client_handler.h NM/ss_handler.h
	$(CC) $(CFLAGS) -c NM/main.c -o $@

NM/client_handler.o: NM/client_handler.c NM/client_handler.h Common/requests.h Common/responses.h Common/loggers.h Common/network_config.h NM/request_handlers/create_request_handler.h
	$(CC) $(CFLAGS) -c NM/client_handler.c -o $@

NM/ss_handler.o: NM/ss_handler.c NM/ss_handler.h Common/requests.h Common/responses.h NM/ss_info.h Common/loggers.h Common/network_config.h
	$(CC) $(CFLAGS) -c NM/ss_handler.c -o $@

NM/request_handlers/create_request_handler.o: NM/request_handlers/create_request_handler.h NM/request_handlers/create_request_handler.c Common/responses.h Common/loggers.h Common/path_utils.h NM/ss_info.h
	$(CC) $(CFLAGS) -c NM/request_handlers/create_request_handler.c -o $@

NM/ss_info.o: NM/ss_info.c NM/ss_info.h NM/trie.h
	$(CC) $(CFLAGS) -c NM/ss_info.c -o $@

NM/trie.o: NM/trie.c NM/trie.h
	$(CC) $(CFLAGS) -c NM/trie.c -o $@

Common/requests.o: Common/requests.c Common/requests.h Common/network_utils.h
	$(CC) $(CFLAGS) -c Common/requests.c -o $@

Common/responses.o: Common/responses.c Common/responses.h Common/network_utils.h
	$(CC) $(CFLAGS) -c Common/responses.c -o $@

Common/network_utils.o: Common/network_utils.c Common/network_utils.h
	$(CC) $(CFLAGS) -c Common/network_utils.c -o $@

Common/loggers.o: Common/loggers.c Common/loggers.h Common/colors.h Common/responses.h
	$(CC) $(CFLAGS) -c Common/loggers.c -o $@

Common/path_utils.o: Common/path_utils.c Common/path_utils.h
	$(CC) $(CFLAGS) -c Common/path_utils.c -o $@