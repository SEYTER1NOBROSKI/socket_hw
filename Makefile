CC = gcc

all: server client server_proxy client_proxy

server: server.c
	$(CC) server.c -o server

client: client.c
	$(CC) client.c -o client

server_proxy: server_proxy.c
	$(CC) server_proxy.c -o server_proxy

client_proxy: client_proxy.c
	$(CC) client_proxy.c -o client_proxy

clean:
	rm -f server client server_proxy client_proxy