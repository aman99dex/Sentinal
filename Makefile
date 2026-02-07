CC = gcc
CFLAGS = -Wall -Wextra -O2

all: server client
server: server.c protocol.c netio.c
	$(CC) $(CFLAGS)  server.c protocol.c netio.c -o server
client: client.c protocol.c netio.c
	$(CC) $(CFLAGS) client.c protocol.c netio.c -o client

clean:
	rm -f server client
