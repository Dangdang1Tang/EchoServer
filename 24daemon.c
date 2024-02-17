/*
会话:就是一个终端(tty),包含多个进程组.

进程组:一组相关联的进程的集合,比如:
1.打开终端后,bash就是一个进程组; 
2.执行"ls | wc -l",ls和wc是一个进程组.
3.执行./hello,hello是一个进程组,hello中fork出来的子进程也属于这个进程组.


setsid: 
setsid()系统调用会按照下列步骤创建一个新会话。
1.调用进程成为新会话的首进程和该会话中新进程组的首进程。调用进程的进程组ID和会话ID会被设置成该进程的进程ID。
2.调用进程没有控制终端。所有之前到控制终端的连接都会被断开。
如果调用进程是一个进程组首进程，那么 setsid()调用会报出 EPERM 错误。
如果没有这个约束的话，进程组组长就能够将其自身迁移至另一个（新的）会话中了，而该进程组的其他成员则仍然位于原来的会话中。
（不会创建一个新进程组，因为根据定义，进程组首进程的进程组 ID 已经与其进程 ID 一样了。）这会破坏会话和进程组之间严格的
两级层次，因此一个进程组的所有成员必须属于同一个会话。


守护进程:
守护进程是在后台运行不受控制终端控制的进程,通常情况下守护进程在系统启动时自动运行.
守护进程的名称通常以d结尾,比如sshd,xinetd,crond,vsftpd等.

创建守护进程的步骤:
1.调用fork(),创建新进程,它将会是未来的守护进程.
2.在父进程中调用exit,保证子进程不是进程组组长.(子进程继承了父进程的进程组id,子进程的进程id和继承来的进程组id一定不同,相同就是进程组组长,反之不是)
3.调用setsid创建新的会话.
4.将当前目录改为根目录.(因为守护进程的运行目录不能被卸载,这样做是为了方便卸载文件系统)
5.将标准输入,标准输出,标准错误重定型到/dev/null.()


*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0)

int setup_daemon(void);

int main(int argc, char* argv[])
{
    //setup_daemon();
    daemon(0,0);   //库函数提供了daemon接口可以直接调用来创建一个守护进程,参数1为nochdir,参数2为noclose.如果都填1则不改变目录也不关闭文件描述符.
    
    //走到这里的时候就只剩下守护进程了,可以在这里进入死循环,来循环的执行某些任务.
    
    printf("daemon out ...\n"); //这句无法输出,因为标准输出已经重定向了
    for(;;);    //死循环
    return 0;
}

int setup_daemon(void)
{
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        ERR_EXIT("fork error");
    }
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    setsid();
    chdir("/");
    int i;
    for (i=0;i<3;++i)
    {
        close(i);   //关闭文件描述符
    }
    open("/dev/null",O_RDWR);   //打开文件,占用最小的空闲文件描述符(0)
    dup(0); //复制0文件描述符,占用最小的空闲文件描述符(1)
    dup(0); //复制0文件描述符,占用最小的空闲文件描述符(2)
    return 0;
}


