.PHONY:clean all
CC=gcc
BIN=01byte_order 02echosrv 02echocli 03echosrv_multi_progress 04p2pcli 04p2psrv 05echosrv_sticky_package_readn 05echocli_sticky_package_writen\
06echosrv_sticky_package_packet 06echocli_sticky_package_packet 07echosrv_sticky_recv_peek_readline 07echocli_sticky_recv_peek_readline\
08echosrv_defunct 08echocli_defunct1 08echocli_defunct2
all:$(BIN)
$BIN:01byte_order.o  02echosrv.o 02echocli 03echosrv_multi_progress 04p2pcli 04p2psrv 05echosrv_sticky_package_readn 05echocli_sticky_package_writen\
06echosrv_sticky_package_packet 06echocli_sticky_package_packet 07echosrv_sticky_recv_peek_readline 07echocli_sticky_recv_peek_readline\
08echosrv_defunct 08echocli_defunct1 08echocli_defunct2
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
