/*

一文讲透计算机的“中断”:https://zhuanlan.zhihu.com/p/360548214
软中断与软件中断:https://zhuanlan.zhihu.com/p/360683396
软中断和信号是什么关系？:https://www.zhihu.com/question/33822078/answer/58619346

中断,软中断,软件中断,信号这几个概念有些混乱.
暂且这样认为:

中断:
是一个泛化的概念:CPU停下当前的工作任务，去处理其他事情，处理完后回来继续执行刚才的任务.

外部中断(硬件中断):
1.可屏蔽中断：通过INTR线向CPU请求的中断，主要来自外部设备如硬盘，打印机，网卡等。此类中断并不会影响系统运行，可随时处理，甚至不处理，所以名为可屏蔽中断。
2.不可屏蔽中断：通过NMI线向CPU请求的中断，如电源掉电，硬件线路故障等。这里不可屏蔽的意思不是不可以屏蔽，不建议屏蔽，而是问题太大，屏蔽不了，不能屏蔽的意思。
注：INTR和NMI都是CPU的引脚


内部中断(软中断,软件中断,异常):
1.陷阱：是一种有意的，预先安排的异常事件，一般是在编写程序时故意设下的陷阱指令，而后执行到陷阱指令后，CPU将会调用特定程序进行相应的处理，处理结束后返回到陷阱指令的下一条指令。如系统调用，程序调试功能等。
尽管我们平时写程序时似乎并没有设下陷阱，那是因为平常所用的高级语言对底层的指令进行了太多层的抽象封装，已看不到底层的实现，但其实是存在的。例如printf函数，最底层的实现中会有一条int 0x80指令，这就是一条陷阱指令，使用0x80号中断进行系统调用。
2.故障：故障是在引起故障的指令被执行，但还没有执行结束时，CPU检测到的一类的意外事件。出错时交由故障处理程序处理，如果能处理修正这个错误，就将控制返回到引起故障的指令即CPU重新执这条指令。如果不能处理就报错。
常见的故障为缺页，当CPU引用的虚拟地址对应的物理页不存在时就会发生故障。缺页异常是能够修正的，有着专门的缺页处理程序，它会将缺失的物理页从磁盘中重新调进主存。而后再次执行引起故障的指令时便能够顺利执行了。
3.终止：执行指令的过程中发生了致命错误，不可修复，程序无法继续运行，只能终止，通常会是一些硬件的错误。终止处理程序不会将控制返回给原程序，而是直接终止原程序

软中断:
Linux选择把一些hardirq的耗时处理逻辑拆分/提交到一个专门的内核线程中去延迟/异步执行。这个“提交”的动作就是发起一次softirq（Linux软中断），专门的处理线程叫softirqd。

软件中断:
从软件中断指令而来。在32位x86中，为了实现linux用户态到内核态的切换，linux使用软中断指令“int 0x80”来触发异常，切换CPU特权级，实现系统调用。

所谓的"硬中断","软中断","软件中断",其实是从英文名词翻译过来的东西,而且是不同平台名词的翻译整到了一起,本来毫无关联的东西变成了一套,所以不必特别纠结.

信号:
signal是一种Unix系统层面纯软件的实现，跟中断的特性有一些相似，都是一种异步监听事件的机制，所以称之为“软中断”也合理。
signal跟hardirq/softirq虽然是完全独立的不同机制，但在Linux实现上它们之间还是有少许联系：部分hardirq处理过程中，会向某些进程发送signal。
比如在键盘按下Ctrl+C，会触发一个hardirq，内核处理这个键盘中断时，会给终端前台进程发送一个SIGINT信号。
评论区提示：signal handler的调用时机，是在每次从内核态切换到用户态时。而hardirq的触发就会造成用户态-内核-用户态这么一轮上下文切换，也就会检查并调用signal handler。


信号与中断的相似点:
1.采用了相同的异步通信方式
2.当检测出有信号或中断请求时,都暂停正在执行的程序转而去执行相应的处理程序
3.都在处理完毕后返回到原来的断点
4.对信号或中断都可进行屏蔽

信号与中断的区别:
1.中断有优先级,而信号没有优先级,所有的信号都是平等的
2.信号处理程序是在用户态下运行的,而中断处理程序是在核心态下运行的
3.中断响应是及时的,而信号响应通常都有较大的时间延迟


进程对信号的三中响应:
1.忽略信号
不采取任何操作,有两个信号不能被忽略:SIGKILL和SIGSTOP
2.捕获并处理信号
内核中断正在执行的代码,转去执行先前注册过的处理函数
3.执行默认操作
默认操作通常是终止进程,这取决于被发送的信号




在 Linux 中，`signal()` 函数是用于注册信号处理函数的函数之一。它的原型如下：

```c
#include <signal.h>

void (*signal(int signum, void (*handler)(int)))(int);
```

该函数用于为指定的信号 `signum` 注册信号处理函数 `handler`，并返回之前该信号的处理函数。参数说明如下：

- `signum`：要处理的信号的编号。可以是任何合法的信号编号，如 `SIGINT`、`SIGTERM` 等。
- `handler`：指向信号处理函数的指针。这个函数应该接受一个整数参数，表示信号编号。

`signal()` 函数的行为在不同的系统中可能略有不同。在某些系统中，它可能返回先前注册的信号处理函数的指针，而在其他系统中，它可能返回固定值或者不返回任何值。

使用 `signal()` 函数注册信号处理函数的示例如下：

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// 信号处理函数
void signal_handler(int signum) {
    printf("Received signal %d\n", signum);
}

int main() {
    // 注册 SIGINT 的信号处理函数
    signal(SIGINT, signal_handler);

    printf("Signal handling example, press Ctrl+C to send SIGINT...\n");

    // 进入一个无限循环，等待信号
    while (1) {
        // do nothing
    }

    return 0;
}
```

在这个示例中，`signal_handler` 函数是我们定义的用于处理信号的函数。在 `main()` 函数中，我们通过调用 `signal(SIGINT, signal_handler)` 注册了 `signal_handler` 作为 `SIGINT` 信号的处理函数。当用户按下 `Ctrl+C` 时，就会向程序发送 `SIGINT` 信号，触发注册的信号处理函数执行。

请注意，`signal()` 函数在一些系统中被认为是过时的，建议使用 `sigaction()` 函数来代替。`sigaction()` 提供了更加灵活和可靠的信号处理机制。
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

void handler(int sig);
int main(int argc, char * argv[])
{
    __sighandler_t oldhandler;
    oldhandler = signal(SIGINT, handler);
    if(oldhandler == SIG_ERR)        //绑定ctrl+c的处理程序
    {
        ERR_EXIT("signal error");
    }
    while (getchar()!='\n') //循环等待字符输入,直到输入回车
    {
    }
    //if(signal(SIGINT, oldhandler) == SIG_ERR) //恢复默认的ctrl+c处理程序
    if(signal(SIGINT, SIG_DFL) == SIG_ERR)  //同上一句
    {
        ERR_EXIT("signal error");
    }

    for (;;);
    return 0;
}

void handler(int sig)
{
    printf("recv signal, sig=%d\n",sig);
}
