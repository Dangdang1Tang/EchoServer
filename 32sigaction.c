/*
sigaction

sa_mask 字段定义了一组信号，在调用由 sa_handler 所定义的处理器程序时将阻塞该组信号。
当调用信号处理器程序时，会在调用信号处理器之前，将该组信号中当前未处于进程掩码之列的任何信号自动添加到进程掩码中。
这些信号将保留在进程掩码中，直至信号处理器函数返回，届时将自动删除这些信号。
利用 sa_mask 字段可指定一组信号，不允许它们中断此处理器程序的执行。
此外，引发对处理器程序调用的信号将自动添加到进程信号掩码中。
这意味着，当正在执行处理器程序时，如果同一个信号实例第二次抵达，信号处理器程序将不会递归中断自己。
由于不会对遭阻塞的信号进行排队处理，如果在处理器程序执行过程中重复产生这些信号中的任何信号，（稍后）对信号的传递将是一次性的。

sa_flags 字段是一个位掩码，指定用于控制信号处理过程的各种选项。该字段包含的位如下（可以相或（|））。
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
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    __sigaddset(&act.sa_mask, SIGQUIT); //SIGINT的处理函数执行期间阻塞SIGQUIT信号
    act.sa_flags = 0;

    if (sigaction(SIGINT, &act, NULL) < 0)
    {
        ERR_EXIT("sigaction error");
    }
    for(;;)
    {
        pause();
    }
    return 0;
}

void handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("recv sig=%d\n",sig);
    }
    sleep(5);
}
