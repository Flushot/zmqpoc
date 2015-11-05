CFLAGS = -I/opt/local/include -L/opt/local/lib

all: server

server: server.c
	$(CC) $(CFLAGS) server.c -lzmq -o server

clean:
	rm -f server
