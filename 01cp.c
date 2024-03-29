#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//宏写成这样是因为：1.宏不能有多条语句 2.把宏的内容包起来，防止被上下文分割，比如if
#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)
    
int main(int argc, char *argv[])
{
    int infd;
    int outfd;
    if (argc != 3)
    {
        fprintf(stderr, "usage %s src dst\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    infd = open(argv[1], O_RDONLY);
    if (infd == -1)
    {
        ERR_EXIT("open src error");
    } 
    outfd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (outfd == -1)
    {
        ERR_EXIT("open dst error");
    }

    char inbuf[1024];
    int nread;
    while ((nread = read(infd, inbuf, 1024)) > 0)
    {
        write(outfd, inbuf, nread);
    }
    close(infd);
    close(outfd);
    return 0;
}