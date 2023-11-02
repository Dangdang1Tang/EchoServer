#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while (0);

//fcntl 操纵文件描述符，改变已打开文件的属性


int main(int argc, char* argv[])
{
    int fd;
    //1.复制文件描述符
    fd = open("test.txt", O_WRONLY | O_CREAT | O_APPEND);
    close(1);
    if (fcntl(fd, F_DUPFD, 0) < 0)  //从0开始搜索空闲文件描述符
    {
        ERR_EXIT("dup fd error");
    }
    printf("hello\n");

    //2.修改文件描述符状态
    char buf[1024] = {0};
    int flags = fcntl(0, F_GETFL, 0);   //先获取标准输入文件描述符当前状态
    if (flags == -1)
    {
        ERR_EXIT("GETFL error");
    }

    //设置文件描述符非阻塞
    int ret = fcntl(0, F_SETFL, flags | O_NONBLOCK);    //设置标准输入非阻塞
    if (ret == -1)
    {
        ERR_EXIT("SETFL error");
    }
    //读非阻塞文件描述符
    ret = read(0, buf, 1024);
    if (ret == -1)
    {
        perror("read error");
    }

    //清除文件描述符非阻塞
    ret = fcntl(0, F_SETFL, flags & (~O_NONBLOCK));
    ret = read(0, buf, 1024);
    printf("%s", buf);
    return 0;
}
 