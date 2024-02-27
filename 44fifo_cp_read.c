/*
读管道,写入Makefile2
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
    int outfd;
    outfd = open("Makefile2", O_WRONLY | O_CREAT | O_TRUNC);
    if (outfd == -1)
    {
        ERR_EXIT("open error");
    }
    int infd;
    infd = open("tp", O_RDONLY);
    if (infd == -1)
    {
        ERR_EXIT("open error");
    }
    char buf[1024];
    int n;
    while((n=read(infd, buf, sizeof(buf))) > 0)
    {
        write(outfd, buf, n);
    }
    close(infd);
    close(outfd);

    return 0;
}


