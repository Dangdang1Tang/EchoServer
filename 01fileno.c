#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    //fileno 文件指针->文件描述符
    printf("fileno(stdin)=%d\n",fileno(stdin));
    printf("fileno(stdout)=%d\n",fileno(stdout));
    printf("fileno(stderr)=%d\n",fileno(stderr));
    return 0;
}
