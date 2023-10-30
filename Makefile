.PHONY:clean all
CC=gcc
BIN=01fileno
all:$(BIN)
$BIN:01fileno.o
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
