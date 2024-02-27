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
    int fd;
    //fd = open("fifo1", O_WRONLY);
    fd = open("fifo1", O_WRONLY | O_NONBLOCK, 0644);
    if (fd == -1)
    {
        ERR_EXIT("open error");
    }
    printf("open success\n");
    return 0;
}


