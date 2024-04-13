/**
 * 不再以定长的方式收发数据，而是加上一个头部协议，头部当中规定了数据包的长度，发送的数据包是长度+包体。
 * 对方接受时分两次接收，先接收长度（4字节），然后接收对应长度的数据。其中自定义的readn和writen在未达成目的时会阻塞，帮助保证读/写到完整的包体。
 * 这样就将消息与消息进行了边界的区分。
*/

#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
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
    int listenfd;
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        return 1;
    }

    //绑定地址
    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(5188);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //htonl可以省略，因为INADDR_ANY的值是全0
    // srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //同上
    // inet_aton("127.0.0.1", &srv_addr.sin_addr); //同上

    int on = 1;
    int setsock_rst = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (setsock_rst < 0)
    {
        return 5;
    }

    int bind_rst = bind(listenfd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
    if (bind_rst < 0)
    {
        return 2;
    }
    //listen 可以3次握手了，与客户端建立连接，放到已连接队列
    int listen_rst = listen(listenfd, SOMAXCONN);
    if (listen_rst < 0)
    {
        return 3;
    }

    //accept 从已连接队列中选择一个连接accept,返回一个主动套接字
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    int conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
    if (conn < 0)
    {
        return 4;   
    }
    printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));


    struct packet recvbuf;
    while(1)
    {
        memset(&recvbuf, 0, sizeof(recvbuf));
        int ret = readn(conn, &recvbuf.len, 4);     //先读出包体的长度
        if (ret == -1)
        {
            perror("read");
            exit(6);
        }
        else if (ret < 4)
        {
            printf("client close\n");
            break;
        }
        else if(ret == -1)
        {
            perror("read");
            exit(5);
        }
        int n = ntohl(recvbuf.len);
        ret = readn(conn, &recvbuf.buf, n);
        if (ret == -1)
        {
            perror("read");
            exit(7);
        }
        else if (ret < 4)
        {
            printf("client close\n");
            break;
        }
        else if(ret == -1)
        {
            perror("read");
            exit(8);
        }
        fputs(recvbuf.buf, stdout);
        writen(conn, &recvbuf.buf, n);  //回射回去就不用写长度了
    }
    close(conn);
    close(listenfd);

    return 0;
}