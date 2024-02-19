/*
为了增强程序的稳定性，在信号处理函数中应使用可重入函数.


可重入函数（reentrant function）是指在同一时间可以被多个线程调用，而不会出现竞争条件或数据竞争，也不会影响函数的正确性或结果的一致性。
可重入函数通常是线程安全的，因为它们在执行期间不依赖于全局或静态变量的状态，并且不会修改共享的数据。
因此，无论函数被多少个线程同时调用，都能够产生正确的结果。

不可重入函数（non-reentrant function）则是指在函数的执行过程中，不允许被中断或再次调用。
这样的函数在多线程或并发环境中可能会出现问题，因为如果在函数执行过程中被中断或再次调用，可能会导致数据不一致性或竞争条件的发生。
不可重入函数通常依赖于全局或静态变量，或者执行过程中使用了共享资源，因此在多线程环境中需要特别小心使用，或者通过加锁等机制来保护其正确性。

总的来说，可重入函数是线程安全的，而不可重入函数则可能在并发环境中出现问题，需要特别小心处理。


由于不可重入函数无法在执行期间被中断或重入，这就意味着它们的执行过程对于多线程环境来说是不安全的。
如果一个函数正在执行，而另一个线程尝试再次调用这个函数，可能会导致数据损坏、不一致性或其他未定义的行为。
因此，不可重入函数被认为是对并发性的一种阻碍。

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

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)


typedef struct
{
    int a;
    int b;
} TEST;

TEST g_data;

void handler(int sig);
int main(int argc, char* argv[])
{
    TEST ones = {1,1};
    TEST zeros = {0,0};
    if (signal(SIGALRM, handler) == SIG_ERR)
    {
        ERR_EXIT("signal error");
    }
    alarm(1);   //1秒后向自己发送SIGALRM信号
    for(;;)
    {
        g_data = zeros;
        g_data = ones;
    }
    return 0;
}

void handler(int sig)
{
    printf("%d %d\n",g_data.a, g_data.b);   //访问了全局变量,但这个例子不是很好
    alarm(1);
}