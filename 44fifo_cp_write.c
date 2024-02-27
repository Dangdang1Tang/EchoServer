/*
读文件Makefile,写入管道
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
    mkfifo("tp", 0644);
    int infd;
    infd = open("Makefile", O_RDONLY);
    if (infd == -1)
    {
        ERR_EXIT("open");
    }
    int outfd;
    outfd = open("tp", O_WRONLY);
    if (outfd == -1)
    {
        ERR_EXIT("open");
    }
    
    char buf[1024];
    int n;
    while ((n=read(infd, buf, sizeof(buf))) > 0)
    {
        write(outfd,buf,n);
    }
    close(infd);
    close(outfd);
    unlink("tp");       //断开链接,fifo的连接数为0就会被删除. 如果不unlink执行完后tp不会被删除.
    return 0;
}


