/*
kill(pid, sig);
pid > 0: 信号sig发送给进程号==pid的进程
pid == 0:信号sig发送给调用者所在进程组中的每一个进程
pid == -1:信号sig发送给调用者进程有权限发送的每一个进程,除了1号进程与自身之外
pid < -1:信号sig发送给进程组==-pid的每一个进程


raise(sig);
给自己发送信号,等价于:kill(getpid(),sig);

killpg(pgrp,sig);
给进程组发送信号,等价于kill(-pgrp,sig);

sigqueue:
给进程发信号,支持排队,可以附带信息.
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
    if (signal(SIGUSR1, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }
    pid_t pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }

    if (pid == 0)
    {
        //kill(getppid(), SIGUSR1);   //给父进程发送信号
        kill(-getpgrp(),SIGUSR1);   //给整组进程发送信号
        exit(EXIT_SUCCESS);
    }

    int n = 5;

    do
    {
        n = sleep(n); //sleep会被信号打断,被打断会返回剩余时间
    } while (n > 0);
    
    return 0;
}

void handler(int sig)
{
    printf("recv a sig = %d\n", sig);
}