CC = gcc
CFLAGS = -Wall -Wextra -O2

all: server client
server: server.c protocol.c
	$(CC) $(CFLAGS)  server.c protocol.c -o server
client: client.c protocol.c
	$(CC) $(CFLAGS) client.c protocol.c -o client

clean:
	rm -f server client
