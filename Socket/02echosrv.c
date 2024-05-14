/*
在网络编程中，通常使用 `struct sockaddr` 和其特定的派生类型，
比如 `struct sockaddr_in`（用于 IPv4 地址）或 `struct sockaddr_in6`（用于 IPv6 地址），取决于具体的需求。
通常情况下，当你需要处理 IPv4 地址时，你会使用 `struct sockaddr_in`，
而当你需要处理 IPv6 地址时，则使用 `struct sockaddr_in6`。
而 `struct sockaddr` 通常用于函数的参数和返回类型，因为它是一个通用的地址结构，可以适用于多种地址族。

例如，在创建套接字时，通常会使用 `struct sockaddr`，但在填充地址信息时，可能会将其转换为特定的类型，如 `struct sockaddr_in`。具体选择要根据你的网络编程需求和操作系统的支持情况而定。
*/


/*
测试发现，连接建立后：
服务器ctrl+c断开连接，客户端什么都不做，服务器会进入FIN_WAIT2. 即服务器等待客户端发送FIN给服务器.
如果服务器ctrl+c断开连接，客户端也ctrl+c断开连接，服务器会进入TIME_WAIT. 即连接正常断开，服务器确认对方不会发送更多数据，但不确定对方收到了最后一个ACK或之前的数据，服务器需要保存连接一段时间，以应对对方可能要求的重传。
如果服务器ctrl+c断开连接，客户端按回车（读取标准输入发送给服务器）继续发送数据给服务器，服务器不会进入TIME_WAIT,连接断开非常干净.

SIGPIPE
服务器关闭之后，客户端还可以向socket中写入数据，即向一个已经接收FIN的套接字中写入数据是允许的，接收到FIN仅仅代表对方已经不再发送数据。
如果发送数据给对方，但对方的进程已经不存在了，会导致TCP的重置，对方会发送RST的TCP段给我方。
在收到RST段之后，如果再调用write写入就会产生SIGPIPE信号，如果不处理会导致进程终止，对这个信号通常忽略即可。

如果服务器ctrl+c断开连接，客户端按回车（读取标准输入发送给服务器）继续发送数据给服务器，服务器不会进入TIME_WAIT,连接断开非常干净.原因是什么？
在这种情况下，当服务器接收到Ctrl+C信号后，会强制关闭TCP连接，此时TCP协议栈会发送一个RST（Reset）包给客户端，来终止连接。RST包相当于一种“咒语”，它能够使连接立即终止，而不需要像正常连接关闭一样等待一段时间。
当客户端收到RST包后，也会立即关闭TCP连接，而不会进入TIME_WAIT状态。这是因为TIME_WAIT状态是为了确保连接足够长时间处于关闭状态，以便所有未到达的数据都能够被接收方处理完毕。
但是，当连接被强制关闭时，所有未到达的数据都会被丢弃，因此不需要等待这个过程完成，连接可以立即关闭，不会进入TIME_WAIT状态。
总的来说，当TCP连接被强制关闭时，不需要等待连接关闭的过程完成，可以立即关闭连接，从而避免了TIME_WAIT状态的影响。

*/

#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>

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
        int ret = read(conn, recvbuf, sizeof(recvbuf));
        fputs(recvbuf, stdout);
        write(conn, recvbuf, ret);
    }
    close(conn);
    close(listenfd);

    return 0;
}
