/**
 * 父进程只负责接受连接，获得一个已连接套接字后，fork一个子进程来管理这个连接，父进程继续接受其他连接
 * 
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
    
    while (1)
    {
        int conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
        if (conn < 0)
        {
            return 4;   
        }
        printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
        int pid = fork();
        if (pid == -1)
        {
            return 5;
        }
        if (pid == 0)
        {
            close(listenfd);
            char recvbuf[1024];
            while(1)
            {
                memset(recvbuf, 0, sizeof(recvbuf));
                int ret = read(conn, recvbuf, sizeof(recvbuf));
                if (ret == 0)
                {
                    //read返回0，意味着读到了文件尾没有更多数据可读，这里意味着客户端关闭
                    printf("client close\n");
                    break;
                }
                else if(ret == -1)
                {
                    perror("read");
                    exit(1);
                }
                fputs(recvbuf, stdout);
                write(conn, recvbuf, ret);
            }
            exit(0);//客户端关闭后子进程没有存在价值了，子进程退出
        }
        else
        {
            close(conn);
        }
    }

    return 0;
}