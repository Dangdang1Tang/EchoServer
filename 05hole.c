#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0);
    
int main(int argc, char *argv[])
{
    int infd;
    infd = open("hole.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (infd == -1)
    {
        ERR_EXIT("open error:");
    }
    write(infd, "ABCDE", 5);
    int offset = lseek(infd, 32, SEEK_CUR);
    if (offset == -1)
    {
        ERR_EXIT("lseek error");
    }
    write(infd, "hello", 5);
    close(infd);
    return 0;
}