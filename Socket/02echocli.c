#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

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
        write(sock, sendbuf, strlen(sendbuf));
        memset(sendbuf, 0, sizeof(sendbuf));

        memset(recvbuf, 0, sizeof(recvbuf));
        read(sock, recvbuf, sizeof(recvbuf));
        fputs(recvbuf, stdout);
    }
    close(sock);
    return 0;
}