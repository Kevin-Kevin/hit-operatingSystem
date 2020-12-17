/* 
 * linux/kernel/sem.c
 * 
 * (C) 2020 Kevin Zhang
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <unistd.h>
#include <asm/system.h>
#include <linux/kernel.h>
#include <linux/sched.h>
// #include <string.h>

// typedef struct queueNode
// {
//     struct task_struct *task;
//     struct queueNode *next;

// } queueNode;

// typedef struct sem_t
// {
//     int value;
//     queueNode *queue;
//     queueNode *end;
//     char name[10];
// } sem_t;

// sem_t sem[5] =
//     {
//         {0, NULL, NULL, name0},
//         {0, NULL, NULL, name1},
//         {0, NULL, NULL, name2},
//         {0, NULL, NULL, name3},
//         {0, NULL, NULL, name4}
//     };
#define SEMS_SIZE 5
sem_t sem[5] =
    {
        {0, NULL, NULL, "NULL"},
        {0, NULL, NULL, "NULL"},
        {0, NULL, NULL, "NULL"},
        {0, NULL, NULL, "NULL"},
        {0, NULL, NULL, "NULL"},
};

int start = 0;
//  queueNode *addQueue(sem_t *sem, struct task_struct *task)
// {
//     printk("add queue begin");
//     queueNode qNode;
//     qNode.task = task;
//     qNode.next = NULL;

//     sem->queue->next = &qNode;
//     sem->end = &qNode;

//     return 0;
// }

// struct task_struct *deleteQueueAndWakeUpTask(sem_t *sem)
// {
//     queueNode *qNode = sem->queue;
//     sem->queue = sem->queue->next;

//     return qNode->task;
// }

sem_t *sys_sem_open(const char *name, unsigned int value)
{

    int ret = 0;
    int i = 0;
    int index = 0;
    char nihao[5] = "NULL\0";
    char kernelName[10] = "\0";

    // 把 *name 的数据放到 kernelName 中
    kernelName[i] = get_fs_byte(name + i);
    while (get_fs_byte(name + i) != '\0')
    {
        i++;
        kernelName[i] = get_fs_byte(name + i);
    }

    if (start == 0)
    {
        printk("kernelName = [ %s ], nihao = %s,fuck\n ", kernelName, nihao);
        printk("sem name = %s,%s,%s,%s,%s\n", sem[0].name, sem[1].name, sem[2].name, sem[3].name, sem[4].name);
        printk("sem  = %d,%d,%d,%d,%d\n", &sem[0], &sem[1], &sem[2], &sem[3], &sem[4]);
        start = 1;
    }

    // 比较 kernelName 和已存在的信号量
    for (i = 0; i < 5; i++)
    {
        // printk("i = %d,", i);
        //信号量存在,更改信号量的 value
        if (strcmp(sem[i].name, kernelName) == 0)
        {
            printk("[ %s ] sem exit !  \n", kernelName);

            sem[i].value = value;
            sem[i].queue = NULL;
            sem[i].end = sem[i].queue;

            printk("%d : [ %s ] value = %d\n", &sem[i], sem[i].name, sem[i].value);

            return &sem[i];
        }
    }
    if (i == 5)
        printk("sem dose exit ,");
    for (i = 0; i < 5; i++)
    {
        // 信号量不存在，就创建
        if (strcmp(sem[i].name, "NULL\0") == 0)
            break;
    }

    //printk("will creat [ %s ] sem  ,value = %d,sem[i].name = %s\n", kernelName, value, sem[i].name);
    for (index = 0; kernelName[index] != '\0'; index++)
    {
        sem[i].name[index] = kernelName[index];
        //         printk("index = %d",index);
    }
    sem[i].value = value;
    sem[i].queue = NULL;
    sem[i].end = sem[i].queue;
    //     printk(" , i=%d\n",i);
    // 函数 strcpy() 不可以正常工作
    //       strcpy(sem[i].name, kernelName);
    //     printk(" -> after creat [ %s ] sem  ,value = %d,sem[i].name = %s\n", kernelName, sem[i].value, sem[i].name);

    return &sem[i];
}
int sys_sem_wait(sem_t *lsem)
{
    // printk(" sys_sem_wait : %d ,", lsem);
    if (lsem == NULL || lsem < sem || lsem >= sem + 5 * (sizeof(sem_t)))
        return -1;

    // 关闭中断
    cli();
  

    // // 信号量减一
    printk(" before [%d]: %s->value = %d ,", current->pid, lsem->name, lsem->value);
    lsem->value--;
//    printk(" after [%d]: sem = %d ->value = %d \n", current->pid, lsem, lsem->value);

    // // 如果信号量小于0，进程睡眠，加入队列
    if (lsem->value < 0)
    {

        // printk("add queue begin\n");

        queueNode qNode;
        qNode.task = current;
        qNode.next = NULL;

        lsem->end->next = &qNode;
        lsem->end = &qNode;
        //     printk("sys_sem_ wait already add queue\n");
        // 写入信号量

        current->state = TASK_INTERRUPTIBLE;
        // 开启中断
        sti();
  //      printk("[ %d ]: sys_sem_wait sti() !!!\n",lsem);
        schedule();
        return 0;

    }

    // 开启中断
    sti();
    //  printk("sys_sem_wait sti() !!!\n");
    return 0;
}

int sys_sem_post(sem_t *lsem)
{
    int fd = 0;
    int ret = 0;
    if (lsem == NULL || lsem < sem || lsem >= sem + 5 * (sizeof(sem_t)))
        return -1;

  //  printk("  [%d]: sys_sem_post , ", lsem);
    // 关闭中断
    cli();

    // 信号量加一
    lsem->value++;

    // 如果信号量小于等于0，从队列中取出进程唤醒
    if (lsem->value <= 0)
    {
        queueNode *qNode = lsem->queue->next;
        lsem->queue->next = qNode->next;
       // struct task_struct *task = qNode->task;

        qNode->task->state = TASK_RUNNING;
        sti();

        schedule();
            return 0;

    }

    // 开启中断
    sti();
    return 0;
}

int sys_sem_unlink(const char *name)
{
        int i = 0;
        char kernelName[10];
        char strnull[4] = "NULL";

        // 把 *name 的数据放到 kernelName 中
        kernelName[i] = get_fs_byte(name + i);

        while (get_fs_byte(name + i) != '\0')
        {
            i++;
            kernelName[i] = get_fs_byte(name + i);
        }

        // 比较 kernelName 和已存在的信号量
        for (i = 0; i < 5; i++)
        {
            //信号量存在,更改信号量的 value
            if (strcmp(sem[i].name, kernelName) == 0)
            {
                printk("this sem exit !\n");

                sem[i].value = 0;
                sem[i].queue = NULL;
                sem[i].end = NULL;
                strcpy(sem[i].name, strnull);

                return 0;
            }
        }
        printk("[ %s ] sem does not exit !\n", kernelName);
    return 0;
}

/* #define MAX_NAME 20

typedef struct sem_t{
    char name[MAX_NAME];
    unsigned int value;
    struct task_struct *wait_queue;
}sem_t;
 */

/* 
#include <string.h> 
#include <asm/segment.h>  
#include <unistd.h>  
#include <asm/system.h>
#include <linux/kernel.h> 

#define SEMS_SIZE 5

static sem_t sems[SEMS_SIZE] = {
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
    {"", 0, NULL},
};

sem_t* sys_sem_open(const char *name, unsigned int value)
{
    if (name == NULL)
    {
        printk("name == NULL\n");
        return NULL;
    }
    int i, index = -1;
    char temp_name[MAX_NAME];
    for (i = 0; i < MAX_NAME; ++i)
    {
        temp_name[i] = get_fs_byte(name+i);
        if (temp_name[i] == '\0')
            break;
    }
    if (i == 0 || i == MAX_NAME)
    {
        printk("name too long or too short, i = %d\n", i);
        return NULL;
    }

    for (i = 0; i < SEMS_SIZE; ++i)
    {
        if (strcmp(sems[i].name, "") == 0)   //信号量还未被初始化
        {
            index = index == -1 ? i : index;  //只要有一个信号量没使用，index=i而不等于-1
            continue;
        }
        if (strcmp(sems[i].name, temp_name) == 0) //名字符合，返回
            return &sems[i];
    }
    sem_t *res = NULL;
    if (index != -1)  //如果还有空位，那么占据他
    {
        res = &sems[index];
        // printk("before set: name is %s\n", res->name);
        // strcpy(sems[index].name, temp_name);  // 不能使用strcpy，是因为在内核态的原因吗？
        for (i = 0; temp_name[i] != '\0'; ++i)
                sems[index].name[i] = temp_name[i];
        sems[index].name[i] = '\0';
        // printk("after set: name is %s\n", res->name);
        res->value = value;
    }
    else
        printk("no empty slots: index = %d\n", index);
    return res;
}

int sys_sem_wait(sem_t *sem)
{
    if (sem == NULL || sem < sems || sem >= sems + SEMS_SIZE)
        return -1;
    cli();
    while (sem->value == 0)
        sleep_on(&sem->wait_queue);
    sem->value--;
    sti();
    return 0;
}

int sys_sem_post(sem_t *sem)
{    
    if (sem == NULL || sem < sems || sem >= sems + SEMS_SIZE)
        return -1;
    cli();
    wake_up(&sem->wait_queue);
    sem->value++;
    sti();
    return 0;
}

int sys_sem_unlink(const char *name)
{
    if (name == NULL)
        return -1;
    int i; 
    char temp_name[MAX_NAME];
    for (i = 0; i < MAX_NAME; ++i)
    {
        temp_name[i] = get_fs_byte(name+i);
        if (temp_name[i] == '\0')
            break;
    }
    if (i == 0 || i == MAX_NAME)
        return -1;
    temp_name[i] = '\0';

    for (i = 0; i < SEMS_SIZE; ++i) //释放信号量
    {
        if (strcmp(sems[i].name, temp_name))
        {
            sems[i].name[0] = '\0';
            sems[i].value = 0;
            sems[i].wait_queue = NULL;
            return 0;
        }
    }

    return -1;
}
 */