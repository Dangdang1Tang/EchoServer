/*
wait和waitpid的区别
在C语言中，`wait()` 和 `waitpid()` 都是用于等待子进程结束的函数，但它们在使用方式和功能上有一些区别。
1. **wait() 函数**：
   - `wait()` 函数会阻塞调用它的进程，直到其任意一个子进程退出或者一个信号中断了父进程的执行。
   - 当一个子进程退出时，`wait()` 函数会返回被等待子进程的进程ID，并将子进程的退出状态传递给调用进程。
   - 如果调用进程没有任何子进程，或者调用进程没有权限等待任何子进程，或者调用进程收到了一个信号打断了 `wait()` 函数的执行，那么 `wait()` 函数会立即返回 -1，并设置 errno 来指示错误。

2. **waitpid() 函数**：
   - `waitpid()` 函数提供了更加灵活的等待子进程的方式，它允许父进程指定要等待的子进程。
   - 父进程可以通过指定子进程的进程ID来等待特定的子进程退出，也可以使用一些选项来控制 `waitpid()` 函数的行为。
   - `waitpid()` 函数的原型为 `pid_t waitpid(pid_t pid, int *status, int options);`，其中 `pid` 参数指定要等待的子进程的进程ID，`status` 参数用于存储子进程的退出状态，`options` 参数用于控制函数的行为。
   - 如果 `pid` 参数为 -1，那么 `waitpid()` 函数等待任何子进程退出。
   - 如果 `pid` 参数为 0，那么 `waitpid()` 函数等待与调用进程在同一个进程组的任何子进程退出。
   - 如果 `pid` 参数大于 0，那么 `waitpid()` 函数等待指定进程ID的子进程退出。
   - `options` 参数可以是 0，或者通过位掩码设置一些选项，比如 `WNOHANG` 可以使 `waitpid()` 函数非阻塞。

总的来说，`waitpid()` 函数相比于 `wait()` 函数更加灵活，可以等待指定的子进程，也可以设置非阻塞选项，具有更多的控制能力。
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int main(int argc, char * argv[])
{
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }
    if (pid == 0)
    {
        printf("this is child\n");
        //exit(100); //正常退出
        abort();  //异常终止
    }
    printf("this is parent\n");
    int status;
    int ret = waitpid(-1, &status, 0);  //第一个参数是-1,表示等待任一子进程.
                                        //如果第一个参数>0,表示等待指定pid的子进程.
                                        //如果第一个参数==0,表示等待和当前进程进程组id相同的子进程.
                                        //如果第一个参数<-1,表示等待和指定进程组id(参数的绝对值)相同的子进程.

    printf("ret=%d pid=%d\n",ret,pid);
    if (WIFEXITED(status))  //子进程正常退出
    {
        printf("child exited normal, exit status=%d\n",WEXITSTATUS(status));    //打印子进程退出码
    }
    else    //异常终止
    {
        printf("child exited abnormal\n");
        if (WIFSIGNALED(status))    //被信号终止
        {
            printf("exited signal number=%d\n",WTERMSIG(status));
        }
        else if(WIFSTOPPED(status)) //被信号暂停
        {
            printf("stoped signal number=%d\n",WSTOPSIG(status));
        }
    }
    return 0;
}