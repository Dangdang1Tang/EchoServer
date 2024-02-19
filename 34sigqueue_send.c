/*
sigqueue
实时信号的发送使用 sigqueue()系统调用，并且还向信号处理器函数提供了一个附加参数（siginfo_t 结构），
以便其获得信号的伴随数据，以及发送进程的进程 ID 和实际用户 ID。
*/
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage %s pid\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    pid_t pid = atoi(argv[1]);
    union sigval v;
    v.sival_int = 100;
    
    sigqueue(pid, SIGINT, v);
    return 0;
}
