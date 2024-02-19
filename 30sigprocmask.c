/*
sigaction()和 sigprocmask()允许程序指定一组将由进程阻塞的信号.
sigpending()则返回一组目前正在等待送达给进程的信号.

sigprocmask()函数既可修改进程的信号掩码，又可获取现有掩码，或者两重功效兼具。
how 参数指定了 sigprocmask()函数想给信号掩码带来的变化。
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
void printSigset(__sigset_t *set)
{
    int i;
    for(i=1; i<_NSIG; ++i)
    {
        if (__sigismember(set,i))  //检测给定的信号是否在集合中
        {
            printf("%d",1);
        }
        else
        {
            printf("%d",0);
        }
        
    }
    printf("\n");
}
int main(int argc, char* argv[])
{

    if (signal(SIGINT, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }
    if (signal(SIGQUIT, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }

    __sigset_t pset;
    __sigset_t bset;
    sigemptyset(&bset);   //信号集置空
    __sigaddset(&bset, SIGINT); //信号集增加

    sigprocmask(SIG_BLOCK, &bset, NULL);    //阻塞SIGINT,ctrl+c发送的信号会被阻塞,不会立即递达

    for(;;)
    {
        sigpending(&pset);  //返回一组目前正在等待送达给进程的信号
        printSigset(&pset);
        sleep(1);
    }
    return 0;
}

void handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("recv sig=%d\n",sig);
    }

    if (sig == SIGQUIT)
    {
        __sigset_t uset;
        sigemptyset(&uset);
        __sigaddset(&uset, SIGINT);
        sigprocmask(SIG_UNBLOCK, &uset, NULL);  //解除对SIGINT的阻塞
    }
    
}
