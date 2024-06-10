#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct stu
{
    char name[32];
    int age;
} STU;

int main(int argc, char* argv[])
{
    int shmid;
    shmid = shmget(1234, 0, 0);  //打开共享内存时不知道大小，填0即可。打开选项也填0即可。
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    STU *p;
    p = shmat(shmid, NULL, 0);
    if (p == (void*)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    //printf("name=%s,age=%d\n",p->name, p->age);
    shmdt(p);
    return 0;
}