#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char* argv[])
{
    signal(SIGCHLD, SIG_IGN); //避免子进程先结束变成僵尸进程。原理：告诉内核不关心子进程的结束，子进程结束时内核会回收。
    printf("before fork,pid=%d\n",getpid());
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        printf("this is parent, pid=%d\n",getpid());
        sleep(1);
    }
    else if(pid == 0)
    {
        printf("this is child,pid=%d, ppid=%d\n",getpid(),getppid());
    }

    return 0;
}