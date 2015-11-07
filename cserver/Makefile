CFLAGS = -I/opt/local/include -L/opt/local/lib -DDEBUG

.PHONY: run_valgrind

all: server

server: server.c
	$(CC) $(CFLAGS) server.c -lzmq -o server

run_valgrind: server
	valgrind --tool=memcheck --leak-check=full --suppressions=vg.supp ./server

clean:
	rm -f server
