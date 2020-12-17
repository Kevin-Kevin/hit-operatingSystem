

#define __LIBRARY__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#include <errno.h>
#include <string.h>

#include <unistd.h>

_syscall2(sem_t *, sem_open, char *, name, int, value);
_syscall1(int, sem_wait, sem_t *, sem);

_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, char *, name);

int main(void)
{
    sem_t *empty;
    sem_t *full;
    sem_t *mutex;
    int fd = 0, fd_output;
    int ret = 0, end = 0;
    int i = 0, index = 0;

    int changedend = 0;
    int input = 0;
    int output = 0;
    char str_output[20], str_itoa[10];

    int next;

    empty = sem_open("empty", 9);
    printf("empty = %d\n", empty);

    full = sem_open("full", 0);
    printf("full = %d\n", full);

    mutex = sem_open("mutex", 1);
    printf("mutex = %d\n", mutex);

    fd = open("./buffer.c", O_RDWR | O_CREAT | O_TRUNC);
    fd_output = open("./output.txt", O_RDWR | O_CREAT | O_TRUNC);

    lseek(fd, 9 * sizeof(int), SEEK_SET);
    ret = write(fd, &end, sizeof(int));
    if (ret == -1)
    {
        printf("write error at start [errno] = %d\n", errno);
    }
    printf("fd = %d \n", fd);
    if (!fork())
    {

        for (i = 0; i <= 150; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);

            ret = lseek(fd, 9 * sizeof(int), SEEK_SET);

            ret = read(fd, &end, sizeof(int));

            if (end < 9)
            {
                end++;

                lseek(fd, (end - 1) * sizeof(int), SEEK_SET);
                write(fd, &i, sizeof(int));

                lseek(fd, 9 * sizeof(int), SEEK_SET);
                write(fd, &end, sizeof(int));

                lseek(fd, 9 * sizeof(int), SEEK_SET);
                read(fd, &changedend, sizeof(int));

                lseek(fd, (end - 1) * sizeof(int), SEEK_SET);
                read(fd, &input, sizeof(int));
                printf("input = %d , ", input);
            }
            else
            {
                printf("producer input error, queue is full");
                return -1;
            }

            sem_post(mutex);

            sem_post(full);
        }
        exit(0);
    }

    /* // three consumers */
    for (i = 0; i < 3; i++)
    {

        if (!fork())
        {
            /*        //    printf("1th consumer pid = %d\n", getpid()); */

            /*    //  printf("consumer pid = %d\n", getpid()); */

            printf("[ %d ]: consumer ,empty = %d, full = %d , mutex = %d\n", getpid(), empty, full, mutex);

            while (index < 200)
            {
                /*   // printf("consumer \n");

        // 查看有没有资源，没有就睡觉 */
                sem_wait(full);
                /*    // 使用 buffer.c 之前上锁 */
                sem_wait(mutex);

                /*     // 读取 buffer.c

        // 读取 end */
                lseek(fd, 9 * sizeof(int), SEEK_SET);
                read(fd, &end, sizeof(int));

                /*    // 读取数字 */
                lseek(fd, 0, SEEK_SET);
                read(fd, &output, sizeof(int));

                /*    // 队列向前移动一位 */
                for (i = 0; i < end - 1; i++)
                {
                    /*     // 读取下一位 */
                    lseek(fd, (i + 1) * sizeof(int), SEEK_SET);
                    read(fd, &next, sizeof(int));

                    /*  // 写入下一位到当前位 */
                    lseek(fd, i * sizeof(int), SEEK_SET);
                    write(fd, &next, sizeof(int));
                }

                end--;
                /* //写入end */
                lseek(fd, 9 * sizeof(int), SEEK_SET);
                write(fd, &end, sizeof(int));
                /* // 读取 end */
                lseek(fd, 9 * sizeof(int), SEEK_SET);
                read(fd, &changedend, sizeof(int));
                /* //      printf("changed end = %d\n",changedend); */

                printf("%d : %d\n", getpid(), output);
                sprintf(str_output, "%d : %d\n\0", getpid(), output);

                lseek(fd_output, 0, SEEK_END);
                ret = write(fd_output, str_output, strlen(str_output));

                /* // 解锁 */
                sem_post(mutex);

                sem_post(empty);
                index++;
            }
            exit(0);
        }
    }

    fd = wait(NULL);
    printf("[%d] exit\n", fd);

    fd = wait(NULL);
    printf("[%d] exit\n", fd);
    fd = wait(NULL);
    printf("[%d] exit\n", fd);
    fd = wait(NULL);
    printf("[%d] exit\n", fd);

    /*    // 删除信号量 */
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");

    return 0;
}