/*
管道读写规则:
当没有数据可读时:
    O_NONBLOCK disable: read调用阻塞,即进程暂停执行,一直等到有数据来为止.
    O_NONBLOCK enable: read调用返回-1,errno值为EAGAIN.
当管道写满时:
    O_NONBLOCK disable: write调用阻塞
    O_NONBLOCK enable:  write调用返回-1,errno值为EAGAIN.
如果所有管道写端对应的文件描述符被关闭,则read返回0.
如果所有管道读端对应的文件描述符被关闭,则write操作会产生信号SIGPIPE.
当要写入的数据量不大于PIPE_BUF时,linux将保证写入的原子性.
当要写入的数据量大于PIPE_BUF时,linux将不再保证写入的原子性.
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
        sleep(3);   //延迟3秒写入数据,默认情况下管道的读方会阻塞,直到数据写入.
        close(pipefd[0]);
        write(pipefd[1], "hello", sizeof("hello")); //父进程在管道写入hello
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }

    close(pipefd[1]);
    char buf[10] = {0};

    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);  //将管道读端设为非阻塞.
    int ret = read(pipefd[0],buf, sizeof(buf));   //子进程在管道读出内容
    if (ret == -1)
    {
        ERR_EXIT("read error");
    }

    printf("recv data=%s\n",buf);

    return 0;
}

