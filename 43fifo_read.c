/*
命名管道(FIFO)
管道应用的一个限制就是只能在有共同祖先的进程间通信.
如果我们想在不相关的进程之间交换数据,可以使用FIFO文件来做这项工作,它经常被称为命名管道.
命名管道是一种特殊类型的文件.

创建一个命名管道:
命名管道可以从命令行上创建,命令行方法是使用下面这个命令:
$mkfifo filename

命名管道也可以从程序里创建,相关函数有:
int mkfifo(const char* filename, mode_t mode);


命名管道与匿名管道:
匿名管道由pipe函数创建并打开.
命名管道由mkfifo函数创建,打开用open.
FIFO(命名管道)与pipe(匿名管道)之间唯一的区别在于他们撞见与打开的方式不同,一旦这些工作完成以后,他们具有相同的语义.

命名管道的打开规则:
如果当前打开操作是为读而打开FIFO时
    O_NONBLOCK disable: 阻塞直到有相应进程为写而打开FIFO
    O_NONBLOCK enable: 立刻返回成功
如果当前打开操作是为写而打开FIFO时
    O_NONBLOCK disable: 阻塞直到有相应进程为读而打开该FIFO
    O_NONBLOCK enable: 立刻返回失败,错误码为ENXIO

*/

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
    //fd = open("fifo1",O_RDONLY);  //默认是阻塞模式
    fd = open("fifo1",O_RDONLY | O_NONBLOCK);   //设为非阻塞模式
    if (fd == -1)
    {
        ERR_EXIT("open error");
    }
    printf("open success\n");
    return 0;
}


