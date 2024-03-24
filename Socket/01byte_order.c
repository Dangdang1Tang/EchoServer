#include<stdio.h>
#include<arpa/inet.h>
//本机是小端字节序
//网络字节序是大端字节序
int main()
{
    //小端字节序
    unsigned int x = 0x12345678;
    unsigned char *p = (unsigned char *)&x;
    printf("%0x %0x %0x %0x\n",p[0], p[1], p[2], p[3]);
    
    //转大端字节序
    unsigned int y = htonl(x);
    p = (unsigned char *)&y;
    printf("%0x %0x %0x %0x\n",p[0], p[1], p[2], p[3]);
    
    //点分十进制ip地址转32位整数
    unsigned long addr = inet_addr("192.168.0.100");
    printf("addr=%u\n",ntohl(addr));

    //
    struct in_addr ipaddr;
    ipaddr.s_addr = addr;
    printf("%s\n", inet_ntoa(ipaddr));
    
    return 0;
}