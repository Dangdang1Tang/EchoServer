#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char* argv[])
{
    signal(SIGCHLD, SIG_IGN);
    int fd;
    fd = open("test.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1)
    {
        ERR_EXIT("open error");
    }

    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }
    if (pid > 0)
    {
        write(fd, "parent",6);
        sleep(1);
    }
    else if (pid == 0)
    {
        write(fd, "child", 5);  //子进程会继承父进程打开的文件描述符，并且和父进程指向同一个文件打开表，
                                //当父进程向这个文件描述符写入内容时，子进程的这个文件描述符指向的文件打开表的当前写入位置也在后移，
                                //所以子进程再向这个文件描述符里写入时不会覆盖掉父进程已经写入的内容。
    }
    return 0;
}