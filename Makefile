CC=gcc -g
CFLAGS=-I.

client: client.c requests.c helpers.c buffer.c parson.c input.c processCommands.c
	$(CC) -o client client.c requests.c helpers.c buffer.c parson.c input.c processCommands.c -Wall

run: client
	./client

clean:
	rm -f *.o client
