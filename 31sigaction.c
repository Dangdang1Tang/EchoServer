/*
sigaction
除去 signal()之外，sigaction()系统调用是设置信号处置的另一选择。虽然 sigaction()的用法比之 signal()更为复杂，但作为回报，也更具灵活性。
尤其是，sigaction()允许在获取信号处置的同时无需将其改变，并且，还可设置各种属性对调用信号处理器程序时的行为施以更加精准的控制。
此外，如 22.7 节所述，在建立信号处理器程序时，sigaction()较之 signal()函数可移植性更佳。

第2个参数最为重要,其中包含了对指定信号的处理,信号所传递的信息,信号处理函数执行过程中应该屏蔽哪些函数等待.
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
__sighandler_t my_signal(int sig, __sighandler_t handler);
int main(int argc, char* argv[])
{
    my_signal(SIGINT,handler);  //调用自定义的signal
    for (;;)
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
}

//用sigaction实现signal
__sighandler_t my_signal(int sig, __sighandler_t handler)
{
    struct sigaction act;
    struct sigaction oldact;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(sig, &act, &oldact) < 0)
    {
        return SIG_ERR;
    }
    return oldact.sa_handler;
}
