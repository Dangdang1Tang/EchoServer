/*
管道是unix中最古老的进程间通信的方式，我们把从一个进程连接到另一个进程的数据流称为一个管道。
本质是内核缓冲区。
管道是半双工的，数据只能向一个方向流动:需要双方通信时,需要建立起两个管道.
只能用于具有共同祖先的进程(具有亲缘关系的进程)之间进行通信.通常,一个管道由一个进程创建,然后该进程调用fork,
此后父子进程之间就可应用该管道.
原因是:父子进程的文件描述符是共享的,有亲缘关系的进程的文件描述符也共享,否则两个进程没办法操纵同一个文件描述符.
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

#include <sys/time.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)

int main(int argc, char* argv[])
{
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
        close(pipefd[0]);
        write(pipefd[1], "hello", sizeof("hello")); //父进程在管道写入hello
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[1]);
    char buf[10] = {0};
    read(pipefd[0],buf, sizeof(buf));   //子进程在管道读出内容
    printf("recv data=%s\n",buf);

    return 0;
}

