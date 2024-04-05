#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <signal.h>

void handler(int sig)
{
    if (sig == SIGPIPE)
    {
        printf("捕捉信号recv sig=%d\n",sig);
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

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(3);
    }
    if (pid == 0)
    {
        char recvbuf[1024] = {0};
        while (1)
        {
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(sock, recvbuf, sizeof(recvbuf));
            if (ret == -1)
            {
                perror("read");
                exit(4);
            }
            else if(ret == 0)
            {
                printf("peer close\n");
                exit(0);
            }
            fputs(recvbuf, stdout);
        }
    }
    else
    {
        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        if (sigaction(SIGPIPE, &act, NULL) < 0)
        {
            perror("sigaction error");
            exit(11);
        }
        char sendbuf[1024] = {0};
        while(1)
        {
            char *p = fgets(sendbuf, sizeof(sendbuf), stdin);
            printf("p:%s",p);
            if (p==NULL)
            {
                printf("fgets ret is null\n");\
                break;
            }
            int ret = write(sock, sendbuf, strlen(sendbuf)); //当对面的套接字关闭后，这里继续向套接字写入数据，操作系统会向应用程序发送一个SIGPIPE信号。
                                //默认情况下，当接收到SIGPIPE信号时，进程会被终止，以防止继续向已经关闭的套接字写入数据造成无法处理的错误。
                                //因此，如果不希望进程被终止，可以通过忽略SIGPIPE信号或者捕获SIGPIPE信号并处理来避免程序异常终止。
                                //例如，可以使用signal(SIGPIPE, SIG_IGN)来忽略SIGPIPE信号，或者使用sigaction函数来捕获和处理SIGPIPE信号。
            printf("ret:%d\n",ret);
            if (ret == -1)
            {
                perror("write");
                exit(5);
            }
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        printf("parent close\n");
        close(sock);
    }
    return 0;
}