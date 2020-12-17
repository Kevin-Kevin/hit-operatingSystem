#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <semaphore.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

#include <errno.h>
#include <string.h>

sem_t *empty, *full, *mutex;
int producer_input_queue(int fd)
{
    int i = 0;
    int end = 0;
    int ret = 0;
    int changedend = 0;
    int input = 0;
    for (i = 0; i <= 500; i++)
    {

        // 查看有没有空余资源，没有就睡觉
        sem_wait(empty);

        // 操作 buffer.c 之前上锁
        sem_wait(mutex);

        // 写入 buffer.c

        // 读取 end
        ret = lseek(fd, 9 * sizeof(int), SEEK_SET);

        ret = read(fd, &end, sizeof(int));

        //printf("get end = %d\n",end);

        if (end < 9)
        {
            end++;

            // 写入数字 i
            lseek(fd, (end - 1) * sizeof(int), SEEK_SET);
            write(fd, &i, sizeof(int));

            // 放入end
            lseek(fd, 9 * sizeof(int), SEEK_SET);
            write(fd, &end, sizeof(int));

            // 读取 end
            lseek(fd, 9 * sizeof(int), SEEK_SET);
            read(fd, &changedend, sizeof(int));
            //     printf("changed end = %d\n",changedend);
            // 读取 input
            lseek(fd, (end - 1) * sizeof(int), SEEK_SET);
            read(fd, &input, sizeof(int));
            //   printf("input = %d\n",input);
        }
        else
        {
            printf("producer input error, queue is full");
            return -1;
        }

        // 解锁
        sem_post(mutex);

        sem_post(full);
    }
    return 0;
}
int consumer_output_queue(int fd)
{

    int output = 0;
    int end = 0;
    int changedend = 0;
    int i = 0;
    int next;

    //  printf("consumer pid = %d\n", getpid());

    while (1)
    {
       // printf("consumer \n");

        // 查看有没有资源，没有就睡觉
        sem_wait(full);
        // 使用 buffer.c 之前上锁
        sem_wait(mutex);

        // 读取 buffer.c

        // 读取 end
        lseek(fd, 9 * sizeof(int), SEEK_SET);
        read(fd, &end, sizeof(int));

        // 读取数字
        lseek(fd, 0, SEEK_SET);
        read(fd, &output, sizeof(int));

        // 队列向前移动一位
        for (i = 0; i < end - 1; i++)
        {
            // 读取下一位
            lseek(fd, (i + 1) * sizeof(int), SEEK_SET);
            read(fd, &next, sizeof(int));

            // 写入下一位到当前位
            lseek(fd, i * sizeof(int), SEEK_SET);
            write(fd, &next, sizeof(int));
        }

        end--;
        //写入end
        lseek(fd, 9 * sizeof(int), SEEK_SET);
        write(fd, &end, sizeof(int));
        // 读取 end
        lseek(fd, 9 * sizeof(int), SEEK_SET);
        read(fd, &changedend, sizeof(int));
        //      printf("changed end = %d\n",changedend);

        printf("%d : %d\n", getpid(), output);

        // 解锁
        sem_post(mutex);

        sem_post(empty);
    }
    exit(0);
}
int main(void)
{
    int fd = 0;
    int ret = 0, end = 0;

    // // 创建信号量 O_CREAT | O_EXCL, 0644,
    // sem_t *empty = sem_open("empty", O_CREAT | O_EXCL, S_IRWXU, 9);

    // sem_t *full = sem_open("full", O_CREAT | O_EXCL, S_IRWXU, 0);

    // sem_t *mutex = sem_open("mutex", O_CREAT | O_EXCL, S_IRWXU, 1);
    // 创建信号量 O_CREAT | O_EXCL, 0644,
    empty = sem_open("empty", O_CREAT, S_IRWXU, 9);
    if(empty == SEM_FAILED)
    {
      printf("errno: %s\n",strerror(errno));
    }

    full = sem_open("full", O_CREAT, S_IRWXU, 0);
    if(full == SEM_FAILED)
    {
      printf("errno: %s\n",strerror(errno));
    }
    mutex = sem_open("mutex", O_CREAT, S_IRWXU, 1);
    if(mutex == SEM_FAILED)
    {
       printf("errno: %s\n",strerror(errno));
    }


    // 打开文件
    fd = open("./buffer.c", O_RDWR | O_CREAT | O_TRUNC);

    // 写入 end
    lseek(fd, 9 * sizeof(int), SEEK_SET);
    ret = write(fd, &end, sizeof(int));
    if (ret == -1)
    {
        printf("write error at start [errno] = %d\n", errno);
    }
    printf("fd = %d \n", fd);

    // 生产者
    if (!fork())
    {
        printf("producer pid = %d\n\n", getpid());
        producer_input_queue(fd);
        exit(0);
    }

    // three consumers
    if (!fork())
    {
        printf("1th consumer pid = %d\n", getpid());

        consumer_output_queue(fd);

        exit(0);
    }
    if (!fork())
    {
        printf("2th consumer pid = %d\n", getpid());

        consumer_output_queue(fd);

        exit(0);
    }
    if (!fork())
    {
        printf("3th consumer pid = %d\n", getpid());

        consumer_output_queue(fd);

        exit(0);
    }

    fd = wait(NULL);
    printf("[%d] exit\n", fd);

    fd = wait(NULL);
    printf("[%d] exit\n", fd);
    fd = wait(NULL);
    printf("[%d] exit\n", fd);
    fd = wait(NULL);
    printf("[%d] exit\n", fd);

    // 删除信号量
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");

    return 0;
}