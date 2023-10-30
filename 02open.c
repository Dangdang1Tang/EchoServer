#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main()
{
    int fd = 0;
    fd = open("test.txt", O_RDONLY);
    if (fd==-1)
    {
        fprintf(stderr,"open error with errno=%d, %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("open success!\n");
    return 0;

}
