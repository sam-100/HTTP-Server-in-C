

server: server.c utils.c
	gcc -o server server.c utils.c

client: client.c
	gcc -o client client.c