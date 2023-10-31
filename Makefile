.PHONY:clean all
CC=gcc
BIN=01fileno 02open 03open 01cp 04lseek 05hole
all:$(BIN)
$BIN:01fileno.o 02open.o 03open.o 01cp.o 04lseek.o 05hole
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
