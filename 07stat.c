#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);  \
        exit(EXIT_FAILURE); \
    } while(0);

#define MAJOR(a) (int)((unsigned short)a >> 8)
#define MINOR(a) (int)((unsigned short)a & 0xFF)

int filetype(struct stat* in_stat)
{
    int flag = 0;
    printf("文件类型:");
    mode_t mode;
    mode = in_stat->st_mode;
    switch (mode & __S_IFMT)
    {
    case __S_IFSOCK:
        printf("socket\n");
        break;
    case __S_IFLNK:
        printf("symbol link\n");
        break;
    case __S_IFREG:
        printf("regular\n");
        break;
    case __S_IFBLK:
        printf("block device\n");
        flag = 1;
        break;
    case __S_IFDIR:
        printf("directory\n");
        break;
    case __S_IFCHR:
        flag = 1;
        printf("character device\n");
        break;
    case __S_IFIFO:
        printf("FIFO\n");
        break;
    default:
        printf("unknown type\n");
        break;
    }
    return flag;
}

int main(int argc, char* argv[])  
{
    if (argc != 2)
    {
        fprintf(stderr, "usage %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    fprintf(stdout, "filename:%s\n",argv[1]);
    struct stat stat_buf;
    if (stat(argv[1], &stat_buf) == -1)
    {
        ERR_EXIT("stat error");
    }
    filetype(&stat_buf);
    fprintf(stdout, "File number, 主设备号:%d, 从设备号:%d, inode号:%d\n", 
        MAJOR(stat_buf.st_dev), MINOR(stat_buf.st_dev), (int)stat_buf.st_ino);

    printf("文件权限,file permission bits=%o\n", stat_buf.st_mode & 0777);

    return 0;
}