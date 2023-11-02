.PHONY:clean all
CC=gcc
BIN=01fileno 02open 03open 01cp 04lseek 05hole 06ls 07stat 08fileshare 09dup 10fcntl 11fcntl_flock
all:$(BIN)
$BIN:01fileno.o 02open.o 03open.o 01cp.o 04lseek.o 05hole.o 06ls.o 07stat.o 08fileshare.o 09dup.o 10fcntl.o 11fcntl_flock
	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
