/*
pause
将进程置为可中断睡眠状态,然后他调用schedule(),使linux进程调度器找到另一个进程来运行.
pause使调用者进程挂起,直到一个信号被捕获.
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

void handler(int sig);
int main(int argc, char* argv[])
{
    if (signal(SIGINT, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }

    for(;;)
    {
        pause();    //让出时间片
        printf("pause return\n");
    }    
    return 0;
}

void handler(int sig)
{
    printf("recv a sig = %d\n", sig);
}