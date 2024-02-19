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
    act.sa_flags = 0;

    __sigset_t s;
    sigemptyset(&s);
    sigaddset(&s, SIGINT);
    sigprocmask(SIG_BLOCK, &s, NULL);   //sigprocmask和sa_mask不同，不仅仅是信号处理函数执行期间生效

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
