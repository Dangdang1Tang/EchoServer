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
    infd = open("Makefile", O_RDONLY);
    char inbuf[1024] = {0};
    int nread = read(infd, inbuf, 5);
    int offset = lseek(infd, 0, SEEK_CUR);
    fprintf(stdout, "nread=%d,offset=%d\n",nread, offset);
    close(infd);
    return 0;
}