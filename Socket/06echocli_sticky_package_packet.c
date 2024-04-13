#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

struct packet
{
    int len;
    char buf[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;   //剩余字节数
    size_t nread;   //已接受字节数
    char *bufp = (char*)buf;
    while (nleft > 0)
    {
        nread = read(fd, bufp, nleft);
        if (nread < 0)
        {
            //read返回值小于0有两种情况：1.被信号中断了 2.出错
            if (errno == EINTR)
            {
                //如果是被信号中断，不认为出错了，可以继续
                continue;
            }
            return -1;
        }
        else if (nread == 0)
        {
            //read返回0意味着对等方关闭了
            break;
        }
        else if (nread > 0)
        {
            bufp += nread;
            nleft -= nread;
        }
    }
    return count - nleft;
}

ssize_t writen(int fd, const void* buf, size_t count)
{
    ssize_t nleft = count;
    ssize_t nwritten;
    char* bufp = (char*)buf;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, buf, nleft) < 0))
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nwritten == 0)
        {
            break;
        }
        else if (nwritten > 0)
        {
            nleft -= nwritten;
            bufp += nwritten; 
        }
    }
    return count - nleft;
}


int main()
{
    //创建套接字
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        return 1;
    }

    //绑定地址
    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(5188);
    // srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //htonl可以省略，因为INADDR_ANY的值是全0
    srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //同上
    // inet_aton("127.0.0.1", &srv_addr.sin_addr); //同上

    int connect_rst = connect(sock, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (connect_rst < 0)
    {
        return 2;
    }

    struct packet sendbuf;
    struct packet recvbuf;
    memset(&sendbuf, 0, sizeof(sendbuf));
    memset(&recvbuf, 0, sizeof(recvbuf));

    while (fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL)
    {
        int n = strlen(sendbuf.buf);
        sendbuf.len = htonl(n);     //发送的整数要主机字节序转网络字节序
        writen(sock, &sendbuf, n + 4);    //包体长n，len标记长4
        memset(&sendbuf, 0, sizeof(sendbuf));

        memset(&recvbuf, 0, sizeof(recvbuf));
        readn(sock, &recvbuf.buf, n);    //为什么还是要用readn，writen？read和write本身不是也支持长度吗？
            //read和write是否会阻塞跟文件描述符是否阻塞有关，但就算文件描述符是阻塞的，
            //read只会在没有数据可读时阻塞，write只会在发送缓冲区已满时阻塞，
            //如果有数据可读但不足指定长度，是不会阻塞的，会立即返回读取到的长度。写应该也是一样？

        fputs(recvbuf.buf, stdout);
    }
    close(sock);
    return 0;
}