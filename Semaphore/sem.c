/*
信号量
互斥：PV在同一进程中
同步：PV在不同进程中

信号量的含义
S>0:S表示可用资源的个数
S=0:表示无可用资源，无等待进程
S<0:|S|表示等待队列中进程个数

semctl
控制信号量集
semid:由semget返回的信号集标识码
semnum:信号集中信号量的序号
cmd:将要采取的动作
cmd:
SETVAL
GETVAL

*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                    (Linux-specific) */
};



int sem_create(key_t key)
{
    int semid;
    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int sem_open(key_t key)
{
    int semid;
    semid = semget(key, 0, 0);
    if (semid == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int sem_setval(int semid, int val)
{
    union semun su;
    su.val = val;
    int ret;
    ret = semctl(semid,0,SETVAL,su);
    if (ret == -1)
    {
        perror("sem setval");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int set_getval(int semid)
{
    int ret;
    ret = semctl(semid, 0, GETVAL, 0);
    if (ret == -1)
    {
        perror("sem getval");
        exit(EXIT_FAILURE);
    }
    return ret;
}

int sem_d(int semid)
{
    int ret;
    ret = semctl(semid, 0, IPC_RMID, 0);
    if (ret == -1)
    {
        perror("sem del");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int sem_p(int semid)
{
    struct sembuf sb = {0, -1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if (ret == -1)
    {
        perror("sem p");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int sem_v(int semid)
{
    struct sembuf sb = {0, 1, 0};
    int ret;
    ret = semop(semid, &sb, 1);
    if (ret == -1)
    {
        perror("sem v");
        exit(EXIT_FAILURE);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int semid;
    semid = sem_create(1234);
    sleep(5);
    sem_d(semid);   //删除信号集

    return 0;
}

