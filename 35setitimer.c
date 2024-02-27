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

#include <sys/time.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)

void handler(int sig)
{
    printf("recv sig=%d\n",sig);
}

int main(int argc, char* argv[])
{
    if (signal(SIGALRM, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }
    struct timeval tv_interval = {1,0}; //1秒0微秒
    struct timeval tv_value = {5,0};    //5秒0微秒
    struct itimerval it;
    it.it_interval = tv_interval;   //之后每隔多久到期一次
    it.it_value = tv_value;     //多久后第一次到期
    setitimer(ITIMER_REAL, &it, NULL);

    int t = 0;
    int i;
    for(i=0;i<100000;i++)
    {
        ++t;
    }
    struct itimerval oit;
    setitimer(ITIMER_REAL, &it, &oit);  //再次设定时器，oit是旧的定时器里面包含了还剩余的时间
    printf("%d,%d,%d,%d\n",(int)oit.it_interval.tv_sec, (int)oit.it_interval.tv_usec, (int)oit.it_value.tv_sec, (int)oit.it_value.tv_usec);

    for(;;)
    {
        pause();
    }
    return 0;
}

