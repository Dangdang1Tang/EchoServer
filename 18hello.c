#include <unistd.h>
#include <stdio.h>

int main(void)
{
    printf("hello ...pid=%d\n", getpid());
    return 0;
}