all: 
	gcc -pthread -o server server.c && ./server
client:
	gcc -pthread -o client client.c && ./client