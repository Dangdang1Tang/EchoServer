/*
alarm
参数 seconds 表示定时器到期的秒数。到期时，会向调用进程发送 SIGALRM 信号。
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
    if (signal(SIGALRM, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }
    alarm(1);   //1秒后向自己发送SIGALRM信号
    for(;;)
    {
        pause();    //让出时间片
        printf("pause return\n");
    }    
    return 0;
}

void handler(int sig)
{
    printf("recv a sig = %d\n", sig);
}