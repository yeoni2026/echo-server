.PHONY: all clean run_server run_client

CC = gcc
CFLAGS = -Wall -Wextra -O2

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c start_routine.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c