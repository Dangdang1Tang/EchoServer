/*
读到'\n'作为一个数据包的结尾，来解决粘包问题。
先通过recv的peek功能将socket缓冲区的数据peek到内存中，然后遍历寻找'\n'。
如果找到了说明有一个完整的数据包，将这个数据包read到buf中，返回。
如果没找到说明不够完整的数据包，将已经peek的数据读到buf中，继续peek，直到找到'\n'。


read() 是标准的 POSIX 函数，在 UNIX 系统中用于从文件描述符（包括套接字）读取数据。
recv() 是套接字库函数，在网络编程中特别针对套接字操作而设计，用于从套接字接收数

如果你主要进行网络编程，推荐使用 recv()；
而如果你需要与文件描述符（包括套接字）交互，并且更关心 POSIX 标准的可移植性，那么可以选择使用 read()。

ssize_t recv(int sockfd, void *buf, size_t len, int flags);

flags 参数可以用来设置一些标志位，以控制接收操作的行为
MSG_PEEK：
如果设置了这个标志，recv() 函数将查看套接字接收缓冲区中的数据，但是不会将数据从缓冲区中移除。
这意味着数据仍然保留在缓冲区中，可以在后续的 recv() 调用中再次读取。
通常用于查看接收缓冲区中的数据，而不影响后续的接收操作。

MSG_OOB:
标志用于接收带外数据（Out-of-Band data）。
带外数据是一种特殊的数据，它们通常用于在正常数据流之外发送紧急消息或控制信息。

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

//只读出来，不从socket接收缓冲区里删除
ssize_t recv_peek(int sockfd, void* buf, size_t len)
{
    while (1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if (ret == -1 && errno == EINTR)
        {
            continue;
        }
        return ret;
    }
}

//先读到内存中，然后遍历看是不是完整的包（有\n），如果是就读到buf中返回，如果不是就读到buf中循环这个过程，直到读到\n为止。
ssize_t readline(int sockfd, void *buf, size_t line_maxlen)
{
    int ret;
    int npeek;
    char *bufp = buf;
    int nleft = line_maxlen;
    while(1)
    {
        ret = recv_peek(sockfd, bufp, nleft);
        if (ret < 0)    //说明出错了，这里不进行被信号中断的判断，因为recv_peek里面已经判断了。
        {
            return ret;
        }
        else if (ret == 0) //对方关闭了套接字
        {
            return ret;
        }
        npeek = ret;    //peek到的长度
        int i;
        for (i = 0; i < npeek; ++i)
        {
            if (bufp[i] == '\n')
            {
                ret = readn(sockfd, bufp, i+1);
                if (ret != i+1)
                {
                    exit(EXIT_FAILURE);
                }
                return ret;
            }
        }
        //peek到的数据里没有'\n',继续peek
        nleft -= npeek;
        ret = readn(sockfd, bufp, npeek);   //已经peek到的先读到buf中
        if (ret != npeek)
        {
            //既然已经peek到了这些，没道理read不到这些
            exit(EXIT_FAILURE);
        }
        bufp += npeek;
    }
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

    char recvbuf[1024];
    while(1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = readline(conn, recvbuf, sizeof(recvbuf)); //如果读不到'\n'就会阻塞在这里，
                                    //但由于输入都是一行一行输入的，所以实际上每次都能读到'\n'
                                    //除非数据超过发送缓冲区长度，导致了两次发送，才会有一个非常短暂的阻塞。
        if (ret == 0)
        {
            printf("client close\n");
            break;
        }
        fputs(recvbuf, stdout);
        write(conn, recvbuf, strlen(recvbuf));
    }
    close(conn);
    close(listenfd);

    return 0;
}