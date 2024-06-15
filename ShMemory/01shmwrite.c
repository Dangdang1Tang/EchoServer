/*
sh命令：ipcs
ipcs 是一个用于显示 System V IPC（进程间通信）设施状态的命令，包括消息队列、共享内存段和信号量。
它可以帮助你查看系统中当前存在的 IPC 资源及其相关信息。
ipcs 命令的常用选项
-m：显示共享内存段的信息。
-q：显示消息队列的信息。
-s：显示信号量的信息。
-a：显示所有可用的 IPC 资源信息（这是默认选项）。

shmget：创建共享内存

shmat：将共享内存段连接到进程地址空间
shmaddr为NULL,内核自动选择一个地址。
shmaddr不为NULL,且shmflg无SHM_RND标记，则以shmaddr为链接地址。
shmaddr不为NULL,且shmflg设置了SHM_RND标记，则连接的地址会自动向下调整为SHMLBA的整数倍。
shmflg=SHM_RDONLY,表示连接操作用来只读共享内存。

shmctl 函数是 Unix 系统中用于控制共享内存段的一种系统调用。
它可以执行多种操作，例如获取共享内存段的状态、设置共享内存段的属性以及标记共享内存段以便将其删除。
该函数通常与共享内存相关的其他函数（如 shmget、shmat 和 shmdt）一起使用。
shmid:由shmget返回的共享内存标识码
cmd:将要采取的动作（有三个可取值）
buf:指向一个保存着共享内存的模式状态和访问权限的数据结构
可选cmd:
IPC_STAT:把shmid_ds结构中的数据设置为共享内存的当前关联值
IPC_SET:在进程有足够权限的前提下，把共享内存的当前关联值设置为shmid_ds数据结构中给出的值
IPC_RMID:删除共享内存段

*/


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct stu
{
    char name[32];
    int age;
} STU;

int main(int argc, char* argv[])
{
    int shmid;
    shmid = shmget(1234, sizeof(STU), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    STU *p;
    p = shmat(shmid, NULL, 0);
    if (p == (void*)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    strcpy(p->name, "lisi");
    p->age = 20;

    while (1)
    {
        //假设对方读走之后会在头部写入一个quit，我方就可以删除共享内存了
        if (memcmp(p, "quit", 4) == 0)
        {
            break;
        }
    }
    shmdt(p);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}