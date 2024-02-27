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
    printf("recv a sig=%d\n", sig);
}

int main(int argc, char* argv[])
{
    signal(SIGPIPE, handler);   //绑定SIGPIPE信号处理函数
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        ERR_EXIT("pipe error");
    }
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }

    if (pid > 0)
    {
        close(pipefd[0]);   //父进程关闭管道读端
        exit(EXIT_SUCCESS);
    }

    close(pipefd[0]);   //子进程关闭管道读端
    sleep(1);
    int ret = write(pipefd[1],"hello", sizeof("hello"));   //子进程向管道写入内容,会写入失败,并给此进程发送SIGPIPE信号
    if (ret == -1)
    {
        printf("write error\n");
    }

    return 0;
}

