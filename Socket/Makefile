.PHONY:clean all
CC=gcc
BIN=01byte_order 02echosrv 02echocli 03echosrv_multi_progress 04p2pcli 04p2psrv
all:$(BIN)
$BIN:01byte_order.o  02echosrv.o 02echocli 03echosrv_multi_progress 04p2pcli 04p2psrv
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
