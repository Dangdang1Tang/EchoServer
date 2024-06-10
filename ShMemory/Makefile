.PHONY:clean all
CC=gcc
CFLAGS=-Wall -g
BIN=01shmwrite 01shmread
all:$(BIN)
#第1种方式：每个目标可执行文件单独写一条规则
# 01shmread:01shmread.o
# 	$(CC) $(CFLAGS) $^ -o $@
# 01shmwrite:01shmwrite.o
# 	$(CC) $(CFLAGS) $^ -o $@ 

#第2种方式：各目标可执行文件用同一条规则，但是需要指出依赖哪些.o文件，否则不会执行.c生成.o
# $(BIN):01shmwrite.o 01shmread.o
# 	$(CC) $(CFLAGS) $@.o -o $@ 

# 第3种方式：使用同一条规则，且自动找出.c转换为同名.o作为依赖。
src=$(wildcard *.c)
$(BIN):$(patsubst %.c,%.o,$(src))
	$(CC) $(CFLAGS) $@.o -o $@ 


%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o $(BIN)
