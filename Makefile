.PHONY:clean all
CC=gcc
BIN=01fileno 02open 03open 01cp
all:$(BIN)
$BIN:01fileno.o 02open.o 03open.o 01cp.o
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
