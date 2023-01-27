CC = gcc

all: client server

debug: CFLAGS += -g
debug: client server

client: client.o 
	${CC} client.o -o client -lpthread

server: server.o account.o
	${CC} server.o account.o -o server
account.o: account.c
	${CC} -c account.c
client.o: client.c
	${CC} -c client.c 

server.o: server.c
	${CC} -c server.c

clean:
	rm -f *.o *~