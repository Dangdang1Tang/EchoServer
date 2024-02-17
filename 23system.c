#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0)

int my_system(const char* command)      //实现一个system函数
{
    pid_t pid;
    int status;
    if (command == NULL)
    {
        return 1;
    }
    if ((pid = fork()) < 0)
    {
        status = -1;
    }
    else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", command, NULL);    //子进程替换为shell程序
        exit(127);
    }
    else
    {
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno == EINTR)     //如果是被信号打断导致waitpid失败,则继续waitpid
            {
                continue;
            }
            status = -1;    //waitpid失败了,break
            break;
        }
    }
    return 0;
}

int main(int argc, char * argv[])
{
    //system("ls -l | wc -l");
    my_system("ls -l | wc -l"); //使用自定义的system函数
    return 0;
}