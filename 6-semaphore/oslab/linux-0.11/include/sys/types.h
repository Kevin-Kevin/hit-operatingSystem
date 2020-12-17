/*
 * @Author: your name
 * @Date: 2020-10-07 23:26:00
 * @LastEditTime: 2020-10-07 23:26:01
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /6-semaphore/oslab/linux-0.11/include/sys/types.h
 */
#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef _TIME_T
#define _TIME_T
typedef long time_t;
#endif

#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef long ptrdiff_t;
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

typedef int pid_t;
typedef unsigned short uid_t;
typedef unsigned char gid_t;
typedef unsigned short dev_t;
typedef unsigned short ino_t;
typedef unsigned short mode_t;
typedef unsigned short umode_t;
typedef unsigned char nlink_t;
typedef int daddr_t;
typedef long off_t;
typedef unsigned char u_char;
typedef unsigned short ushort;

typedef struct { int quot,rem; } div_t;
typedef struct { long quot,rem; } ldiv_t;

struct ustat {
	daddr_t f_tfree;
	ino_t f_tinode;
	char f_fname[6];
	char f_fpack[6];
};

typedef struct queueNode
{
    struct task_struct *task;
    struct queueNode *next;

} queueNode;

typedef struct sem_t
{
    int value;
    queueNode *queue;
    
    queueNode *end;
    char name[10];
} sem_t;
 

 #define MAX_NAME 20

// typedef struct sem_t{
//     char name[MAX_NAME];
//     unsigned int value;
//     struct task_struct *wait_queue;
// }sem_t; 
#endif
