#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <errno.h>

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

    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        writen(sock, sendbuf, sizeof(sendbuf)); //为了让接收端不阻塞等待数据，直接发送定长包，满足接收端readn一次读到。
                                                //这样做的问题是：增加了发送数据的长度，增加网络负担。
        memset(sendbuf, 0, sizeof(sendbuf));

        memset(recvbuf, 0, sizeof(recvbuf));
        readn(sock, recvbuf, sizeof(recvbuf));
        fputs(recvbuf, stdout);
    }
    close(sock);
    return 0;
}