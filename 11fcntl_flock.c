#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


int main(int argc, char* argv[])
{
    int fd;
    fd = open("test2.txt",O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    lock.l_type = F_WRLCK;      //锁的类型
    lock.l_whence = SEEK_SET;   //偏移起点
    lock.l_start = 0;           //偏移量
    lock.l_len = 0;             //锁多少字节，0表示全部

    //if (fcntl(fd, F_SETLK, &lock) == 0)
    if (fcntl(fd, F_SETLKW, &lock) == 0)    //带W的加锁，如果不能获得锁，就会阻塞直到拿到锁
    {
        printf("lock success\n");
        printf("press any key to unlock\n");
        getchar();
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_UNLCK, &lock) == 0)
        {
            printf("unlock success");
        }
        else
        {
            perror("unlock error");
        }
    }
    else
    {
        perror("lock fail");
        exit(EXIT_FAILURE);
    }   
    return 0;
}