/*
粘包问题：
什么是粘包问题？
粘包问题（Packet Concatenation）是在网络通信中常见的问题，特别是在使用 TCP 协议进行通信时可能会出现。
它指的是发送方连续发送的多个小数据包被接收方一次性接收，导致数据粘在一起，难以区分和解析。
TCP是面向流的协议，它将数据视为一个字节流进行传输，而不是像UDP那样将数据分割成数据包。
因此，在传输过程中，TCP会将数据切割成不同大小的数据段进行传输，并且在接收端进行重组，这可能导致数据粘包。
粘包情况：
1.应用层一次数据的大小超过了缓冲区大小，就会分多次发送，发送过程中又有新的数据要发送，就会一起发送，两个数据粘到了一起。
2.ip层发现超过了MTU就会对数据进行分片，分别发送的数据可能会分到一个片中。


什么时候发送方会把缓冲区的数据发送给对面？
发送方将缓冲区的数据发送给对方的时机取决于多种因素，主要包括以下几点：
1. **缓冲区填充满**: 当发送方的发送缓冲区填满时，发送方会开始发送数据给对方。这是因为发送缓冲区满时，发送方无法继续向其中写入新的数据，必须先发送部分数据释放空间，以便继续发送数据。
2. **Nagle算法**: Nagle算法是一种优化TCP传输性能的算法。当发送方有数据要发送时，如果发送缓冲区中有数据尚未发送，而且发送方已经发送的数据量未达到一个MSS（最大报文段长度），发送方会等待一段时间，以便将多个小数据包合并成一个大的数据包一起发送。这样可以减少网络中的数据包数量，提高传输效率。
3. **立即发送**: 有些情况下，发送方可能会选择立即发送数据，而不是等待Nagle算法的触发条件。例如，当发送方发送的数据是用户交互性的，需要尽快到达对方并得到响应时，发送方可能会选择立即发送数据。
4. **应用程序调用**: 当应用程序主动调用发送数据的函数（如 `send` 函数）时，发送方会立即将缓冲区的数据发送给对方。应用程序可以根据自己的需求和策略来控制数据发送的时机。
综上所述，发送方将缓冲区的数据发送给对方的时机取决于发送方的发送策略、TCP协议的机制以及应用程序的调用行为等因素。


粘包问题怎么解决？
粘包问题是在网络通信中常见的问题，特别是在TCP协议中。它指的是发送方连续发送的多个小数据包被接收方一次性接收，导致数据粘在一起，难以区分和解析。解决粘包问题可以采取以下几种方法：
1. **固定长度消息**: 发送方在发送消息之前固定消息的长度，并且接收方按照固定长度进行消息的接收和解析。这样可以确保每个消息都有固定的长度，从而避免粘包问题。但是，如果消息长度不固定或者消息长度很长，这种方法可能会浪费网络带宽。
2. **消息边界标记**: 在消息之间插入特定的边界标记，如换行符 `\n`、回车符 `\r` 等，接收方根据边界标记来识别消息的结束。这种方法需要发送方和接收方都能够约定好消息的边界标记，并且确保消息中不会包含与边界标记相同的内容。
3. **消息头部信息**: 在消息中添加头部信息，用于指示消息的长度或者其他必要的信息。接收方首先读取头部信息，根据头部信息来确定消息的长度，然后再读取对应长度的消息内容。这种方法需要发送方和接收方都能够协商好头部信息的格式和内容。
4. **使用消息长度字段**: 在消息中添加一个固定长度的字段来表示消息的长度，接收方首先读取消息长度字段，然后再读取对应长度的消息内容。这种方法也需要发送方和接收方协商好消息长度字段的格式和长度。
5. **使用应用层协议**: 在应用层实现自定义的协议，定义消息的格式、长度和解析规则。这种方法可以根据具体的需求和场景来自由定义协议，但需要额外的开发工作和协商。
选择哪种方法取决于具体的应用场景和需求，有时也需要结合多种方法来解决复杂的粘包问题。
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

    char recvbuf[1024];
    while(1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        int ret = readn(conn, recvbuf, sizeof(recvbuf));    //一次读取1024个字节，如果套接字接收缓冲区内不足1024个字节，就会阻塞等数据。
        fputs(recvbuf, stdout);
        writen(conn, recvbuf, ret);
    }
    close(conn);
    close(listenfd);

    return 0;
}