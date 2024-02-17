
/* ========
进程的五种终止方式
一.正常退出
1.从main函数返回
2.调用exit
3.调用_exit
二.异常退出
4.调用abort
5.由信号终止


在C语言中，`exit` 和 `_exit` 是两个不同的函数，它们有着不同的作用和行为。
1. **exit()**：
   - `exit` 函数是C标准库提供的函数之一，位于 `<stdlib.h>` 头文件中。
   - 当调用 `exit` 函数时，会正常地终止程序的执行，并执行一系列的清理操作，包括调用注册的退出处理函数（通过 `atexit` 注册的函数）。
   - `exit` 函数的原型是 `void exit(int status)`，其中 `status` 参数表示程序的退出状态码，一般约定 0 表示正常退出，非零值表示异常退出。

2. **_exit()**：
   - `_exit` 函数也是用于终止程序的执行，但是它是一个系统调用，不同于 `exit` 函数。
   - `_exit` 函数不会执行 `exit` 函数所做的清理工作，也不会调用任何注册的退出处理函数。它会立即终止进程的执行，不进行任何缓冲或清理操作。
   - `_exit` 函数的原型是 `void _exit(int status)`，与 `exit` 函数一样，`status` 参数表示程序的退出状态码。

总的来说，`exit` 是标准C库提供的函数，执行了一系列的清理操作，而 `_exit` 是一个系统调用，直接终止进程的执行，不进行额外的清理操作。在一般情况下，建议使用 `exit` 函数，除非你确实需要绕过标准的清理流程和退出处理函数。
===========*/

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

int main(int argc, char* argv[])
{
    printf("hello world");  //没有\n不会立即把缓冲区的数据输出
    //_exit(0);           //_exit不会做清除缓冲区的操作,所以不会输出到标准输出

    //fflush(stdout);
    exit(0);    //exit会立即输出到标准输出
}
    

