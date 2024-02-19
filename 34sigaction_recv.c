/*
可以像标准信号一样，使用常规（单参数）信号处理器来处理实时信号。
此外，也可以用带有 3 个参数的信号处理器函数来处理实时信号，其建立则会用到 SA_SIGINFO 标志.
一旦采用了 SA_SIGINFO 标志，传递给信号处理器函数的第二个参数将是一个 siginfo_t结构，内含实时信号的附加信息。

如果在使用 sigaction()创建处理器函数时设置了 SA_SIGINFO 标志，那么在收到信号时处
理器函数可以获取该信号的一些附加信息。为获取这一信息，需要将处理器函数声明如下：
如同标准信号处理器函数一样，第 1 个参数 sig 表示信号编号。第 2 个参数 siginfo 是用于
提供信号附加信息的一个结构。该结构会与最后一个参数 ucontext 一起，在下面做详细说明。
因为上述信号处理器函数的原型不同于标准处理器函数，依照 C 语言的类型规则，将无法
利用 sigaction 结构的 sa_handler 字段来指定处理器函数地址。此时需要使用另一个字段：
sa_sigaction。

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

void handler(int, siginfo_t *, void *);
int main(int argc, char* argv[])
{
    struct sigaction act;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

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

void handler(int sig, siginfo_t *info, void *ctx)
{
    printf("recv a sig=%d data=%d\n",sig, info->si_value.sival_int);
}
