/* 
 *  linux/kernel/shareMemory.c
 * 
 *  (c) 2020 by kevin 
 * 
 */


#include <errno.h>
#include <asm/memory.h>




#include <linux/sched.h>
#include <linux/kernel.h>

long int memoryKey[100] = {0};

long int sys_shmget(int key, unsigned int size, int shmflg)
{
    // 如果 key 对应的共享内存已经建立，返回 shmid
    if(memoryKey[key] != 0)
        return memoryKey;
    // 如果 size 超过一页内存的大小，返回-1，并置 errno 为 EINVAL
    if(size > 4*1024*8)
    {
        return -1;
        errno = EINVAL;
    }

    // 获得空闲物理内存
    unsigned long freePageID = get_free_page();
    memoryKey[key] = freePageID;
    return memoryKey[key];
}

void *sys_shmat(long int shmid, const void *shmaddr, int shmflg)
{
    // 忽略参数 shmaddr,shmflg

    // 获取进程虚拟内存中 brk 的虚拟地址
    // get_base 得到基址，current->brk 得到偏移长度  
    unsigned long vitualMemoryAddress = get_base(current->ldt[1]) + current->brk;
    // 建立虚拟地址与物理地址映射关系
    put_page(vitualMemoryAddress, shmid);
    // 建立逻辑地址与虚拟地址映射关系（查找段表）
    // 逻辑地址 = 虚拟地址 - 段基址
    // logicalAddress = vitualMemoryAddress - get_base(current->ldt[1]) = current->brk
    unsigned int logicalAddress = current->brk;
    return logicalAddress;


}