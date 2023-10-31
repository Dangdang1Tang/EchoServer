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
    
int main()
{
    int fd = 0;
    umask(0);
    fd = open("test.txt", O_WRONLY | O_CREAT, 0666);
    if (fd == -1)
    {
        ERR_EXIT("open error");
    }
    printf("open success!\n");
    close(fd);
    return 0;

}
