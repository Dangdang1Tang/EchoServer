#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    signal(SIGCHLD, SIG_IGN); //避免子进程先结束变成僵尸进程。原理：告诉内核不关心子进程的结束，子进程结束时内核会回收。
        //如果父进程先结束，子进程会变成孤儿进程，然后被init进程接管父进程变成init进程。
        //如果子进程先结束，但子进程结束后父进程一直没有处理它的子进程结束信号，子进程就会一直是僵尸进程，直到父进程处理它。
    printf("before fork,pid=%d\n",getpid());    //获取pid
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)    //父进程返回子进程pid
    {
        printf("this is parent, pid=%d\n",getpid());    //获取pid
        sleep(1);
    }
    else if(pid == 0)   //子进程返回0
    {
        printf("this is child,pid=%d, ppid=%d\n",getpid(),getppid());   //获取父进程pid
    }

    return 0;
}