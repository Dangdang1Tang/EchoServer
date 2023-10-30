.PHONY:clean all
CC=gcc
BIN=01fileno 02open
all:$(BIN)
$BIN:01fileno.o 02open.o
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
