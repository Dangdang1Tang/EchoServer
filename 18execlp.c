/*
exec替换进程映像
在进程的创建上unix采用了一个独特的方法,它将进程创建与加载一个新进程映像分离.这样的好处是有更多的余地对两种操作进行管理.
当我们创建了一个进程之后,通常将子进程替换成新的进程映像,者可以用exec系列的函数来进行.当然,exec系列的函数也可以将当前进程替换掉.

$ps
是一个shell程序加载ps程序的过程:
1.shell程序fork创建一个新进程
2.将新进程用ps程序替换(用exec系列函数实现)

*/

// 在C语言中，exec 系列函数是用于在当前进程中执行新程序的一组函数。这些函数通常用于在当前进程中加载并执行一个新的程序映像，取代当前的进程映像。这些函数包括：

// int execl(const char *path, const char *arg0, ... /*, (char *)0 */ );
// int execv(const char *path, char *const argv[]);
// int execle(const char *path, const char *arg0, ... /*, (char *)0, char *const envp[] */ );
// int execve(const char *path, char *const argv[], char *const envp[]);
// int execlp(const char *file, const char *arg0, ... /*, (char *)0 */ );
// int execvp(const char *file, char *const argv[]);
// 这些函数的作用是在当前进程中执行一个新的程序，但是它们的参数和用法略有不同：

// path 或 file 参数是要执行的程序的路径或名称。
// arg0 是新程序的第一个参数，它通常是程序的名称。
// argv 是一个字符串数组，包含了新程序的命令行参数列表。
// envp 是一个字符串数组，包含了新程序执行时的环境变量列表。
// 其中，exec 函数的名称后缀通常用来指示不同的参数传递方式或者搜索路径方式（例如，l 表示可变参数列表，v 表示参数以数组形式传递，p 表示根据 PATH 环境变量搜索可执行文件，e 表示可以自定义环境变量）。

// 这些函数在执行成功时不会返回，而是直接在新的程序中运行。如果发生了错误，这些函数会返回 -1，并设置 errno 来指示错误类型。


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
    printf("pid=%d\n",getpid());    //替换不会改变进程id
    //execlp("ps", "ps", "-x", NULL); //去PATH环境变量指示的路径下搜索ps可执行文件,可以直接成功.
    //execlp("./18hello", "hello", NULL); //去当前目录下搜索18hello可执行文件,也可以成功
    int ret = execlp("18hello", "hello", NULL);   //去PATH环境变量指示的路径下搜索18hello可执行文件,失败.
    if (ret == -1)
    {
        perror("execlp error");
    }
    printf("Exiting main ...\n");
    return 0;
}