/*
I/O模型：
阻塞IO：服务器阻塞在read/recv上直到有数据到达。只能处理一个套接字（一个连接）。
非阻塞IO：把套接字设为非阻塞，循环读套接字，没有数据时直接返回，有数据就处理。可处理多个套接字，浪费CPU。
多进程多线程：每有一个连接就创建一个新的子进程/子线程处理这个连接。进程/线程数量太多，效率差。
信号驱动IO：不好用，不灵活。
IO复用模型：select、poll、epoll。同时监听多个套接字，当其中任意一个套接字处于可读或可写状态时，监听函数就会返回，

select
select IO 模型是一种基于事件驱动的 IO 多路复用模型，它允许一个进程监视多个文件描述符，一旦其中任何一个文件描述符准备好进行 IO 操作（如读取或写入），就通知进程进行相应的 IO 操作。这种模型常见于网络编程中，用于实现高并发的服务器。
在 select 模型中，通常使用 `select` 函数或者其变种（如 `pselect` 或 `poll`）来监视多个文件描述符。这些函数会阻塞进程，直到其中任何一个文件描述符准备好进行 IO 操作或者超时发生。

一般来说，select 模型包含以下步骤：
1. **准备文件描述符集合**：首先，程序会准备一个文件描述符集合，将需要监视的文件描述符添加到集合中。
2. **调用 select 函数**：接下来，程序调用 select 函数，将准备好进行读取、写入或者发生异常检测的文件描述符集合传递给 select 函数。同时还可以设置超时时间，以防止 select 函数永远阻塞。
3. **等待事件发生**：select 函数会阻塞程序，直到监视的文件描述符中的任何一个准备好进行 IO 操作或者超时发生。
4. **处理IO事件**：一旦 select 函数返回，程序就会检查哪些文件描述符已经准备好进行 IO 操作，并对其进行相应的读取、写入或者处理异常。
select IO 模型的优点是简单易用，并且适用于各种平台。但是它也存在一些缺点，比如效率不高，当文件描述符数量很大时，每次调用 select 都需要线性扫描整个文件描述符集合，导致效率下降。此外，select 模型还有文件描述符数量的限制，通常受到操作系统的限制，这限制了它在高并发场景下的应用。


为什么select的nfds参数必须设置为比3个文件描述符集合中所包含的最大文件描述符还要大1，为什么该参数让select变得更有效率？
疑问：既然select只会扫描指定的文件描述符，那么就扫描这些就可以了啊，指定一个nfds有什么意义呢？又不会减少扫描范围。
可能的原因：fd_set是以位掩码的方式实现的，select返回的就绪的文件描述符是一个位掩码，它包含的位是从1到关心的fd中最大的那个+1。
所以不只是要位所有关心的套接字准备一个位，所有中间的不关心的套接字都要准备一个位，只不过这些不关心的位永远不会被置为1。
所以指定nfds可以让select参数直接初始化这个位掩码，这样也算提高了效率？

如果不指定就要遍历3个集合，找出最大的。如果每次都这样，而且频率还不低，勉强算是有效率问题吧。
*/


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

ssize_t readn(int fd, void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nread == 0)
			return count - nleft;

		bufp += nread;
		nleft -= nread;
	}

	return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char *bufp = (char*)buf;

	while (nleft > 0)
	{
		if ((nwritten = write(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				continue;
			return -1;
		}
		else if (nwritten == 0)
			continue;

		bufp += nwritten;
		nleft -= nwritten;
	}

	return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while (1)
	{
		int ret = recv(sockfd, buf, len, MSG_PEEK);
		if (ret == -1 && errno == EINTR)
			continue;
		return ret;
	}
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
	int ret;
	int nread;
	char *bufp = buf;
	int nleft = maxline;
	while (1)
	{
		ret = recv_peek(sockfd, bufp, nleft);
		if (ret < 0)
			return ret;
		else if (ret == 0)
			return ret;

		nread = ret;
		int i;
		for (i=0; i<nread; i++)
		{
			if (bufp[i] == '\n')
			{
				ret = readn(sockfd, bufp, i+1);
				if (ret != i+1)
					exit(EXIT_FAILURE);

				return ret;
			}
		}

		if (nread > nleft)
			exit(EXIT_FAILURE);

		nleft -= nread;
		ret = readn(sockfd, bufp, nread);
		if (ret != nread)
			exit(EXIT_FAILURE);

		bufp += nread;
	}

	return -1;
}

void echo_srv(int conn)
{
	char recvbuf[1024];
        while (1)
        {
                memset(recvbuf, 0, sizeof(recvbuf));
                int ret = readline(conn, recvbuf, 1024);
		if (ret == -1)
			ERR_EXIT("readline");
		if (ret == 0)
		{
			printf("client close\n");
			break;
		}
		
                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf));
        }
}

void handle_sigchld(int sig)
{
/*	wait(NULL);*/
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

int main(void)
{
/*	signal(SIGCHLD, SIG_IGN);*/
	signal(SIGCHLD, handle_sigchld);
	int listenfd;
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
/*	if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)*/
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");*/
	/*inet_aton("127.0.0.1", &servaddr.sin_addr);*/

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");
	if (listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	struct sockaddr_in peeraddr;
	socklen_t peerlen;	
	int conn;

/*
	pid_t pid;
	while (1)
	{
		if ((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
			ERR_EXIT("accept");

		printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));

		pid = fork();
		if (pid == -1)
			ERR_EXIT("fork");
		if (pid == 0)
		{
			close(listenfd);
			echo_srv(conn);
			exit(EXIT_SUCCESS);
		}
		else
			close(conn);
	}
*/

	int i;
	int client[FD_SETSIZE];
	int maxi = 0;

	for (i=0; i<FD_SETSIZE; i++)
		client[i] = -1;

	int nready;
	int maxfd = listenfd;
	fd_set rset;
	fd_set allset;
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);	//把监听套接字加入到allset中
							//之所以有allset是因为传进select每次都会被修改为有数据的套接字，所以需要备份所有需要关心的套接字下次再传。
	while (1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);	//select会遍历完所有关心的套接字之后，才会考虑是不是返回。
												//所以每次返回可能有很多套接字有数据。
		if (nready == -1)
		{
			if (errno == EINTR)
				continue;
			
			ERR_EXIT("select");
		}
		if (nready == 0)
			continue;

		if (FD_ISSET(listenfd, &rset))		//如果监听套接字有数据，说明有新的连接，先把新的连接加入关心列表。
		{
			peerlen = sizeof(peeraddr);
			conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
			if (conn == -1)
				ERR_EXIT("accept");

			for (i=0; i<FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = conn;	//用一个数组管理所有已连接套接字，只用一个变量保存会被下一个覆盖掉。
					if (i > maxi)
						maxi = i;
					break;
				}
			}

			if (i == FD_SETSIZE)
			{
				fprintf(stderr, "too many clients\n");
				exit(EXIT_FAILURE);
			}
			printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
			
			FD_SET(conn, &allset);
			if (conn > maxfd)
				maxfd = conn;	//记录maxfd，需要将maxfd传到select中

			if (--nready <= 0)
				continue;

		}

		for (i=0; i<=maxi; i++)
		{
			conn = client[i];		//遍历所有已连接套接字，看在不在就绪列表里，有数据就处理
			if (conn == -1)
				continue;

			if (FD_ISSET(conn, &rset))
			{
				char recvbuf[1024] = {0};
				int ret = readline(conn, recvbuf, 1024);
                		if (ret == -1)
                        		ERR_EXIT("readline");
                		if (ret == 0)
                		{
                        		printf("client close\n");
					FD_CLR(conn, &allset);
					client[i] = -1;
                		}

                		fputs(recvbuf, stdout);
                		writen(conn, recvbuf, strlen(recvbuf));

				if (--nready <= 0)
					break;
				
			}
		}
	}	
	return 0;
}
