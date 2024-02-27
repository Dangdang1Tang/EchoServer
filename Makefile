.PHONY:clean all
CC=gcc
BIN=01fileno 02open 03open 01cp 04lseek 05hole 06ls 07stat 08fileshare 09dup 10fcntl 11fcntl_flock 12fork \
13fork 14fork 15exit 16atexit 17execve 18execlp 18hello 19execle 19hello 20fcntl_setfd 21wait 22waitpid \
23system 24daemon 25signal 26kill 27pause 28alarm 29reentrant 31sigaction 32sigaction 33sigaction \
34sigaction_recv 34sigqueue_send 35setitimer 36getitimer 37pipe 38ls_wc 39cp 40pipe 41pipe 42pipe 43fifo_read\
43fifo_write 44fifo_cp_read 44fifo_cp_write
all:$(BIN)
$BIN:01fileno.o 02open.o 03open.o 01cp.o 04lseek.o 05hole.o 06ls.o 07stat.o 08fileshare.o 09dup.o 10fcntl.o 11fcntl_flock.o 12fork.o \
13fork.o 14fork.o 15exit.o 16atexit.o 17execve.o 18execlp.o 18hello.o 19execle.o 19hello.o 20fcntl_setfd.o 21wait.o	22waitpid.o \
23system.o 24daemon.o 25signal.o 26kill.o 27pause.o 28alarm.o 29reentrant.o 31sigaction.o 32sigaction.o 33sigaction.o \
34sigaction_recv.o 34sigqueue_send.o 36getitimer.o 37pipe.o 38ls_wc.o 39cp.o 40pipe.o 41pipe.o 42pipe.o 43fifo_read.o\
43fifo_write.o 44fifo_cp_read.o 44fifo_cp_write.o

	$(CC) $< -o $(BIN)
%.o:%.c
	$(CC) -Wall -g -c $< -o $@
clean:
	rm -f *.o $(BIN)
