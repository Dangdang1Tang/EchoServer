#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/*
#define ERR_EXIT(m) \
    perror(m);  \
    exit(EXIT_FAILURE);
*/

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0);
    
int main()
{
    int fd = 0;
    fd = open("test.txt", O_RDONLY);
    
    if (fd==-1)
    {
        //fprintf(stderr,"open error with errno=%d, %s\n", errno, strerror(errno));
        //exit(EXIT_FAILURE);
        ERR_EXIT("open error");
    }
    printf("open success!\n");
    return 0;

}
