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
    close(0);
    open("Makefile", O_RDONLY); //打开Makefile文件,文件描述符存到0上.
    close(1);
    open("Makefile2", O_WRONLY | O_CREAT | O_TRUNC, 0644);  //打开Makefile2文件,文件描述符存到1上.

    execlp("cat", "cat", NULL); //从Makefile读取数据,输出到Makefile2.

    return 0;
}

