/*
close与shutdown的区别？
close：用于关闭任何文件描述符，包括套接字。它立即终止连接并释放文件描述符。既不能再从套接字中读，也不能再向套接字中写。
shutdown：专门用于控制套接字的关闭行为，可以选择性地关闭读、写或读写通道，不会释放文件描述符。

选择close还是shutdown取决于具体需求。对于一般的套接字关闭操作，使用close即可。
如果需要控制数据传输的方向，确保数据发送完毕或部分关闭连接，则应使用shutdown。

比如客户端发送完一波数据后不会再写了，但是希望收到服务器的回复，所以就只关闭写。

shutdown how=1就可以保证对等方接收一个EOF字符（FIN段），而不管其他进程是否已经打开了套接字。
而close不能保证，close直到套接字引用计数减为0时，也就是所有进程（比如子进程）都close了这个套接字才会发送FIN。

如果客户端在ctrl+d时close套接字，服务器又向套接字中写入数据：
客户端会在再次select时报错，因为套接字已经关闭了，select不应该再关心这个套接字。
服务器向对方已经关闭的连接写入数据，会受到对方返回的RST，当服务器收到RST后再次写入数据，就会收到SIGPIPE，如果不处理信号就会进程终止。
就算服务器处理了SIGPIPE信号，此时对方套接字也已经关闭了，服务器不管是对这个套接字读还是写都会失败。
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

void handle_sigpipe(int sig)
{
	printf("recv a sig=%d\n", sig);
}

int main(void)
{
	/*signal(SIGPIPE, SIG_IGN);*/
	signal(SIGPIPE, handle_sigpipe);
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
	FD_SET(listenfd, &allset);
	while (1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready == -1)
		{
			if (errno == EINTR)
				continue;
			
			ERR_EXIT("select");
		}
		if (nready == 0)
			continue;

		if (FD_ISSET(listenfd, &rset))
		{
			peerlen = sizeof(peeraddr);
			conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
			if (conn == -1)
				ERR_EXIT("accept");

			for (i=0; i<FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = conn;
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
				maxfd = conn;

			if (--nready <= 0)
				continue;

		}

		for (i=0; i<=maxi; i++)
		{
			conn = client[i];
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
					close(conn);
                		}

                		fputs(recvbuf, stdout);
				sleep(4);
                		writen(conn, recvbuf, strlen(recvbuf));

				if (--nready <= 0)
					break;
				
			}
		}
	}	
	return 0;
}
