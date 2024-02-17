#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int gval = 100; //全局变量 
int main(int argc, char* argv[])
{
    signal(SIGCHLD, SIG_IGN);

    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }
    if (pid > 0)
    {
        sleep(1);
        printf("this is parent pid=%d childpid=%d, gval=%d\n",getpid(), pid, gval);
        sleep(1);
    }
    else if (pid == 0)
    {
        gval++;         //子进程和父进程原则上不共享变量各自有独立的地址空间，但实际上使用“写时拷贝”机制，即只读时共享，修改时拷贝。
        printf("this is child pid=%d parentpid=%d, gval=%d\n",getpid(), getppid(),gval);
 
    }
    return 0;
}