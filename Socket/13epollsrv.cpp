
/*
epoll和select、poll的区别：
1.相比于select和poll，epoll最大的好处在于它不会随着监听fd的数量的增加而降低效率。
2.内核中的select和poll的实现是采用轮询来处理的，轮询的fd数目越多，自然耗时越多。
3.epoll的实现是基于回调的，如果fd有期望的事件发生就通过回调函数将其加入到epoll就绪队列中，也就是说它只关心活跃的fd，与fd数目无关。
4.内核/用户空间内存拷贝问题，如何让内核把fd消息通知给用户空间呢？在这个问题上select/poll采取了内存拷贝的方法，而epoll采取了共享内存的方式。
5.epoll不仅会告诉应用程序有IO事件到来，还会告诉应用程序相关的信息，这些信息是应用程序填充的，
因此根据这些信息应用程序就能直接定位到相关的事件，而不必遍历整个fd集合。


select：
把需要监听的套接字加入到集合中，然后返回有事件发生的套接字的集合，不过这里的集合使用位mask实现的，
所以需要遍历所有套接字，挨个看这个套接字是否在集合里面（即位与操作是否不是0）.

poll:
把需要监听的套接字构造结构体加入到数组中，传入数组，返回原数组（包含所有套接字），有事件发生的套接字的结构体会被填充事件信息，
但是由于事件信息是填充在原结构体里，所以需要遍历所有套接字，看哪一个结构体被填充了。

epoll：
把需要监听的套接字构造结构体加入到数组中，传入数组，返回新的数组，新数组中剔除了那些没有事件发生的结构体，
所以不需要遍历所有套接字，返回的就是有事件的。


EPOLLLT:level trigger 水平触发
完全靠kernel epoll驱动，应用程序只需要处理从epoll_wait返回的fds，这些fds我们认为他们处于就绪状态。

EPOLLET:edge trigger 边沿触发
此模式下，系统仅仅通知应用程序哪些fds变成了就绪状态，一旦fd变成就绪状态，epoll将不再关注这个fd的任何状态信息（从epoll队列移除），
直到应用程序通过读写操作触发EAGAIN状态，epoll认为这个fd又变为空闲状态，那么epoll又重新关注这个fd的状态变化（重新加入epoll队列）。
随着epoll_wait的返回，队列中的fds是在减少的，所以在大并发的系统中，EPOLLET更有优势，但对程序员的要求也更高。

*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/epoll.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <vector>
#include <algorithm>

typedef std::vector<struct epoll_event> EventList;

#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

void activate_nonblock(int fd)
{
	int ret;
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		ERR_EXIT("fcntl");

	flags |= O_NONBLOCK;
	ret = fcntl(fd, F_SETFL, flags);
	if (ret == -1)
		ERR_EXIT("fcntl");
}

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
	char *bufp = (char*)buf;
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
	int count = 0;
	signal(SIGPIPE, handle_sigpipe);
	signal(SIGCHLD, handle_sigchld);
	int listenfd;
	if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
		ERR_EXIT("setsockopt");

	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");
	if (listen(listenfd, SOMAXCONN) < 0)
		ERR_EXIT("listen");

	std::vector<int> clients;
	int epollfd;
	epollfd = epoll_create1(EPOLL_CLOEXEC);

	struct epoll_event event;
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
	
	EventList events(16);
	struct sockaddr_in peeraddr;
	socklen_t peerlen;
	int conn;
	int i;

	int nready;
	while (1)
	{
		nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
		if (nready == -1)
		{
			if (errno == EINTR)
				continue;
			
			ERR_EXIT("epoll_wait");
		}
		if (nready == 0)
			continue;

		if ((size_t)nready == events.size())
			events.resize(events.size()*2);

		for (i = 0; i < nready; i++)
		{
			if (events[i].data.fd == listenfd)
			{
				peerlen = sizeof(peeraddr);
				conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
				if (conn == -1)
					ERR_EXIT("accept");

				printf("ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
				printf("count = %d\n", ++count);	
				clients.push_back(conn);
				
				activate_nonblock(conn);

				event.data.fd = conn;
				event.events = EPOLLIN | EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);
			}
			else if (events[i].events & EPOLLIN)
			{
				conn = events[i].data.fd;
				if (conn < 0)
					continue;

				char recvbuf[1024] = {0};
				int ret = readline(conn, recvbuf, 1024);
				if (ret == -1)
					ERR_EXIT("readline");
				if (ret == 0)
				{
					printf("client close\n");
					close(conn);

					event = events[i];
					epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
					clients.erase(std::remove(clients.begin(), clients.end(), conn), clients.end());
				}

				fputs(recvbuf, stdout);
				writen(conn, recvbuf, strlen(recvbuf));
			}

		}	
	}

	return 0;
}
