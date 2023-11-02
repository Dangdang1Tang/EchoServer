#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0);
    
int main(int argc, char* argv[])
{
    int fd;
    fd = open("test.txt", O_WRONLY | O_CREAT | O_APPEND);
    if (fd == -1)
    {
        ERR_EXIT("open error");
    }
    //close(1);
    //dup(fd);    //复制一个文件描述符，找一个空闲的文件描述符填进去
    dup2(fd, 1);  //指定要填入的文件描述符，如果不空闲会先close
    printf("hello\n");
    return 0;
}