#include <unistd.h>
#include <stdio.h>
extern char** environ;

int main(void)
{
    printf("hello ...pid=%d\n", getpid());
    int i;
    for (i=0; environ[i]!=NULL; ++i)
    {
        printf("%s\n", environ[i]);     //这里只输出了指定的环境变量(AA和BB),即指定环境变量后就不再从shell继承环境变量?
    }
    return 0;
}