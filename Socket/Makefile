.PHONY:clean all
CC=gcc
BIN=01byte_order 02echosrv 02echocli
all:$(BIN)
$BIN:01byte_order.o  02echosrv.o 02echocli
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
