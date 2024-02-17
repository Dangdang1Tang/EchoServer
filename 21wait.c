/*
SIGCHLD
1.当子进程退出的时候,内核会向父进程发送SIGCHLD信号,子进程的退出是个异步事件(子进程可以在父进程运行的任何时刻终止)
2.子进程退出时,内核将子进程置为僵尸状态,这个进程成为僵尸进程,它只保留最小的一些内核数据结构,以便父进程查询子进程的退出状态.
3.父进程查询子进程的退出状态可以用wait/waitpid函数
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char * argv[])
{
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }
    if (pid == 0)
    {
        printf("this is child\n");
        //exit(100); //正常退出
        abort();  //异常终止
    }
    printf("this is parent\n");
    int status;
    int ret = wait(&status);  //如果子进程已经退出，wait() 函数会立即返回，返回子进程的退出状态信息
                    //如果子进程还没有退出，调用 wait() 函数会导致父进程阻塞，直到一个子进程退出或者收到一个信号打断等情况。父进程会一直处于等待状态，直到有子进程退出。
    printf("ret=%d pid=%d\n",ret,pid);  //ret是子进程的pid,这里打印的两个值相同
    if (WIFEXITED(status))  //子进程正常退出
    {
        printf("child exited normal, exit status=%d\n",WEXITSTATUS(status));    //打印子进程退出码
    }
    else    //异常终止
    {
        printf("child exited abnormal\n");
        if (WIFSIGNALED(status))    //被信号终止
        {
            printf("exited signal number=%d\n",WTERMSIG(status));
        }
        else if(WIFSTOPPED(status)) //被信号暂停
        {
            printf("stoped signal number=%d\n",WSTOPSIG(status));
        }
    }
    return 0;
}