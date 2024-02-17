#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)

void my_exit1(void)
{
    printf("my exit1 ...\n");
}

void my_exit2(void)
{
    printf("my exit2 ...\n");
}

int main(int argc, char* argv[])
{
    atexit(my_exit1);
    atexit(my_exit2);   //通过atexit安装终止程序,终止程序的调用顺序与安装顺序相反
    exit(0);
}
    

