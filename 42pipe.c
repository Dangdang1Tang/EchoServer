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
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        ERR_EXIT("pipe error");
    }

    int ret;
    int count=0;

    int flag = fcntl(pipefd[1], F_GETFL);
    fcntl(pipefd[1], F_SETFL, flag | O_NONBLOCK); //把管道文件描述符改成非阻塞模式,写满后直接返回-1.

    while (1)
    {

        ret = write(pipefd[1], "A", 1); //管道默认是阻塞模式,写满后再写就阻塞了.
        if (ret == -1)
        {
            break;
        }
        ++count;
    }
    printf("pipe maxsize=%d\n",count);  //写65536个字符就满了,即管道容量是64k字节
    return 0;
}

