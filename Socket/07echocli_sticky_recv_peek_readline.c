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
        writen(sock, sendbuf, strlen(sendbuf));
        int ret = readline(sock, recvbuf, sizeof(recvbuf));
        if (ret == -1)
        {
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            printf("client close\n");
            break;
        }
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    close(sock);
    return 0;
}