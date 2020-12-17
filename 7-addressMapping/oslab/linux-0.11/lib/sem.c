/*
 * @Author: your name
 * @Date: 2020-10-08 00:12:44
 * @LastEditTime: 2020-10-08 00:26:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /6-semaphore/oslab/linux-0.11/lib/sem.c
 */
/*
 *  linux/lib/sem.c
 *
 *  (C) 2020  Kevin Zhang
 */

#define __LIBRARY__
#include "unistd.h"


_syscall2(sem_t*,api_sem_open,char*,name,int,value)
 
_syscall1(int,sem_wait,sem_t*,sem)

_syscall1(int,sem_post,sem_t*,sem)

_syscall1(int,sem_unlink,char*,sem)
