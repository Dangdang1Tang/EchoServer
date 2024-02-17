#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int gval = 100;
int main(int argc, char* argv[])
{
    printf("Entering main ...\n");
    int ret = fcntl(1, F_SETFD, FD_CLOEXEC);    //exec时关闭标准输出文件描述符
    if (ret == -1)
    {
        perror("fcntl error");
    }
    ret = execlp("./18hello", "hello", NULL);   //18hellp中的printf不会再输出,因为标准输出被关闭了
    if (ret == -1)
    {
        perror("execlp error");
    }
    printf("Exiting main ...\n");
    return 0;
}