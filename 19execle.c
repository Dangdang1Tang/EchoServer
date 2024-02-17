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
    printf("pid=%d\n",getpid());

    char * const envp[] = {"AA=11","BB=22",NULL};   //指定环境变量
    int ret = execle("./19hello", "hello", NULL, envp); //使用带e的exec函数,把环境变量传进去
    if (ret == -1)
    {
        perror("execlp error");
    }
    printf("Exiting main ...\n");
    return 0;
}