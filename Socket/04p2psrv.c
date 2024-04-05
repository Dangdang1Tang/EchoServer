/**
 * 如果只有一个进程管理一个连接的话，只能单向通信（一方循环读取键盘输入/另一方循环监听连接）
 * 要实现双向通信，每一方都应该有两个进程，一个进程持续监听套接字内容，另一个进程持续监听键盘输入内容。


`read` 和 `fgets` 是 C 语言中用于从文件读取数据的两个函数，它们有以下区别：
1. **函数原型**：
     ssize_t read(int fd, void *buf, size_t count);
     char *fgets(char *str, int size, FILE *stream);
2. **使用场景**：
   - `read` 函数通常用于低级别的文件 I/O 操作，它直接对文件描述符进行操作，因此可以用于任何类型的文件（普通文件、设备文件等）。
   - `fgets` 函数则是标准 I/O 库中的一个函数，通常用于高级别的文本文件读取操作，它对文件流进行操作，主要用于读取文本文件。
3. **行处理**：
   - `read` 函数读取指定字节数的数据，不关心数据中是否包含换行符，因此它不具备行处理的能力。
   - `fgets` 函数从文件中读取一行数据，包括换行符，当读取到换行符或者达到指定的缓冲区大小时会停止读取，因此它可以用于逐行读取文本文件。
4. **返回值**：
   - `read` 函数的返回值是成功读取的字节数，或者在出错时返回 -1。
   - `fgets` 函数的返回值是读取到的字符串的指针（即参数 `str` 的值），如果成功读取到数据，则返回 `str` 的值；如果到达文件末尾或者发生错误，则返回 `NULL`。
5. **错误处理**：
   - `read` 函数通常通过返回值和设置 `errno` 来表示错误信息。
   - `fgets` 函数可以通过返回值和调用 `feof` 和 `ferror` 函数来检查是否到达文件末尾或者发生了错误。
因此，根据具体的需求和使用场景，选择适合的函数来进行文件读取操作。如果需要进行低级别的文件 I/O 操作，可以使用 `read` 函数；如果需要进行文本文件的逐行读取，可以使用 `fgets` 函数。


fgets读取什么时会阻塞？读取什么时不会阻塞？
`fgets` 函数在读取文件时的阻塞行为取决于文件的类型以及文件描述符的设置。下面我将详细说明：

1. **阻塞情况**：
   - **从终端（标准输入）读取**：当 `fgets` 尝试从终端（例如控制台）读取输入时，如果终端设置为行缓冲模式（通常是默认设置），则 `fgets` 会阻塞等待用户输入，直到遇到换行符（用户按下回车键）或者达到指定的缓冲区大小为止。
   - **管道、套接字等读取**：当 `fgets` 尝试从管道、套接字等管道类型的文件描述符中读取数据时，如果没有数据可用（缓冲区为空），则 `fgets` 会阻塞等待，直到有数据可读或者文件结束（EOF）为止。

2. **非阻塞情况**：
   - **从普通文件读取**：当 `fgets` 尝试从普通文件中读取数据时，通常情况下不会阻塞，它会根据文件中的内容进行读取，而不会阻塞等待。即使文件没有数据可读（已经读取到文件末尾），`fgets` 也会立即返回 `NULL`，而不会阻塞等待。
   - **从缓冲区读取**：当 `fgets` 尝试从内存缓冲区中读取数据时，它也不会阻塞等待，它会立即读取缓冲区中的数据并返回。

总之，`fgets` 函数在读取不同类型的文件时可能会有不同的阻塞行为。通常情况下，从终端或者管道中读取时可能会阻塞，而从普通文件或者缓冲区中读取时则不会阻塞。
*/

#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

void handler(int sig)
{
    printf("捕捉信号 sig=%d\n", sig);
    exit(0);
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

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(5);
    }

    if (pid == 0)
    {
        signal(SIGUSR1, handler);   //子进程捕捉信号SIGUSR1，自定义处理函数
        //子进程读取标准输入，发送给对方
        char sendbuf[1024] = {0};
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
        {
            write(conn, sendbuf, strlen(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }
    else
    {
        //父进程读取socket，打印到屏幕上
        char recvbuf[1024];
        while(1)
        {
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(conn, recvbuf, sizeof(recvbuf));
            if (ret == -1)
            {
                exit(6);
            }
            else if (ret == 0)
            {
                printf("peer close\n");
                break;
            }
            fputs(recvbuf, stdout);
        }
        printf("parent close\n");
        kill(pid, SIGUSR1);     //父进程发现对端关闭时，自己退出之前先通知子进程让子进程退出，否则子进程会向已经关闭的连接中写入数据引发错误。
        close(conn);
        close(listenfd);
        exit(0);
    }

    return 0;
}