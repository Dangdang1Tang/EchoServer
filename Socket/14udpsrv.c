/*
udp特点：
1.无连接
2.基于消息的数据传输服务 （区别于tcp:基于流，无边界，有粘包问题）
3.不可靠
4.一般情况下uds更加高效

udp注意点：
1.udp报文可能会丢失，重复
2.udp报文可能会乱序
3.udp缺乏流量控制
4.udp协议数据报文截断
5.recvfrom返回0，不代表连接关闭，因为udp是无连接的
6.icmp异步错误
7.udp connect
8.udp外出接口的确定0


*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

void echo_srv(int sock)
{
	char recvbuf[1024] = {0};
	struct sockaddr_in peeraddr;
	socklen_t peerlen;
	int n;
	while (1)
	{
		peerlen = sizeof(peeraddr);
		memset(recvbuf, 0, sizeof(recvbuf));
		n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&peeraddr, &peerlen);
		if (n == -1)
		{
			if (errno == EINTR)
				continue;
			
			ERR_EXIT("recvfrom");
		}
		else if (n > 0)
		{
			fputs(recvbuf, stdout);
			sendto(sock, recvbuf, n, 0, (struct sockaddr*)&peeraddr, peerlen);
		}
	}

	close(sock);
}

int main(void)
{
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		ERR_EXIT("socket");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		ERR_EXIT("bind");

	echo_srv(sock);
	return 0;
}
