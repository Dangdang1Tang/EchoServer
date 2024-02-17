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

int gval = 100;
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
        printf("this is parent pid=%d childpid=%d, gval=%d\n",getpid(), pid, gval);
    }
    else if (pid == 0)
    {
        char *const args[] = {"ps", NULL};
        execve("/bin/ps", args, NULL);  //将当前进程替换为/bin/ps,参数要用数组传进来
        gval++;
        printf("this is child pid=%d parentpid=%d, gval=%d\n",getpid(), getppid(),gval);
 
    }
    return 0;
}