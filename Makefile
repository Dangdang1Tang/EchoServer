.PHONY:clean all
CC=gcc
BIN=01fileno 02open 03open 01cp 04lseek 05hole 06ls 07stat 08fileshare 09dup 10fcntl 11fcntl_flock 12fork \
13fork 14fork 15exit 16atexit 17execve 18execlp 18hello 19execle 19hello 20fcntl_setfd 21wait 22waitpid \
23system 24daemon
all:$(BIN)
$BIN:01fileno.o 02open.o 03open.o 01cp.o 04lseek.o 05hole.o 06ls.o 07stat.o 08fileshare.o 09dup.o 10fcntl.o 11fcntl_flock.o 12fork.o \
13fork.o 14fork.o 15exit.o 16atexit.o 17execve.o 18execlp.o 18hello.o 19execle.o 19hello.o 20fcntl_setfd.o 21wait.o	22waitpid.o \
23system.o 24daemon.o

	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
