/*
 *  linux/kernel/fork.c
 *
 *  (C) 1991  Linus Torvalds
 */

/*
 *  'fork.c' contains the help-routines for the 'fork' system call
 * (see also system_call.s), and some misc functions ('verify_area').
 * Fork is rather simple, once you get the hang of it, but the memory
 * management can be a bitch. See 'mm/mm.c': 'copy_page_tables()'
 */
#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>


extern void write_verify(unsigned long address);

long last_pid=0;

void verify_area(void * addr,int size)
{
	unsigned long start;

	start = (unsigned long) addr;
	size += start & 0xfff;
	start &= 0xfffff000;
	start += get_base(current->ldt[2]);
	while (size>0) {
		size -= 4096;
		write_verify(start);
		start += 4096;
	}
}

int copy_mem(int nr,struct task_struct * p)
{
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;

	code_limit=get_limit(0x0f);
	data_limit=get_limit(0x17);
	old_code_base = get_base(current->ldt[1]);
	old_data_base = get_base(current->ldt[2]);
	if (old_data_base != old_code_base)
		panic("We don't support separate I&D");
	if (data_limit < code_limit)
		panic("Bad data_limit");
	new_data_base = new_code_base = nr * 0x4000000;
	p->start_code = new_code_base;
	set_base(p->ldt[1],new_code_base);
	set_base(p->ldt[2],new_data_base);
	if (copy_page_tables(old_data_base,new_data_base,data_limit)) {
		printk("free_page_tables: from copy_mem\n");
		free_page_tables(new_data_base,data_limit);
		return -ENOMEM;
	}
	return 0;
}

/*
 *  Ok, this is the main fork-routine. It copies the system process
 * information (task[nr]) and sets up the necessary registers. It
 * also copies the data segment in it's entirety.
 */
extern long first_return_from_kernel(void);
int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss)
{
	struct task_struct *p;
	int i;
	struct file *f;

	p = (struct task_struct *) get_free_page();
	if (!p)
		return -EAGAIN;


	task[nr] = p;
	*p = *current;/* NOTE! this doesn't copy the supervisor stack */
	p->state = TASK_UNINTERRUPTIBLE;
	p->pid = last_pid;
	p->father = current->pid;
	p->counter = p->priority;
	p->signal = 0;
	p->alarm = 0;
	p->leader = 0;/* process leadership doesn't inherit */
	p->utime = p->stime = 0;
	p->cutime = p->cstime = 0;
	p->start_time = jiffies;
/* changed code by kevin */

 	long *kernelStack = (long *)(PAGE_SIZE + (long)p); 
	/* 
	 * 把内核栈做成进程切换之前的样子，即
	 * INT0x80 -> system_call -> schedule() -> switch_to()
	 * 这个过程中形成的堆栈写一遍
	 */
	/* INT0x80 */
 	*(--kernelStack) = ss & 0xffff; 
	*(--kernelStack) = esp;
	*(--kernelStack) = eflags;
	*(--kernelStack) = cs & 0xffff;
	*(--kernelStack) = eip; 
	
	/* system_call */
 	*(--kernelStack) = ds & 0xffff; 
	*(--kernelStack) = es & 0xffff; 
	*(--kernelStack) = fs & 0xffff; 
	*(--kernelStack) = gs & 0xffff; 

	*(--kernelStack) = esi;
	*(--kernelStack) = edi;
	*(--kernelStack) = edx; 
	/* schedule() */

	/* switch_to 弹栈后执行 ret 
	 * ret会继续弹一次栈给 EIP 执行
	 * 所以这次弹出来的要求是函数地址
	 * 如果是经历过切换的老进程，不是新fork出来的，
	 * 那这里的地址会是 schedule() 中 switch_to 的下一条指令
	 * 即 }
	 * 但由于这里新 fork 出来的，这里的地址就改变到另一个函数 
	 * 即 first_return_from_kernel 
	 * 在 first_return_from_kernel 中执行中断返回 iret
	 * 所以这里 schedule() 就不用入栈一些值，因为跳过了       
	 */
 	*(--kernelStack) = (long)first_return_from_kernel;
	
	
	/* switch_to() */
 	*(--kernelStack) = ebp;
	*(--kernelStack) = ecx;
	*(--kernelStack) = ebx;
	*(--kernelStack) = 0;
	 
	/* 进程 PCB 的 内核栈指针 指向 内核栈*/
	p->kernelStack = kernelStack; 

/* over by kevin */


	if (last_task_used_math == current)
		__asm__("clts ; fnsave %0"::"m" (p->tss.i387));
	if (copy_mem(nr,p)) {
		task[nr] = NULL;
		free_page((long) p);
		return -EAGAIN;
	}
	for (i=0; i<NR_OPEN;i++)
		if ((f=p->filp[i]))
			f->f_count++;
	if (current->pwd)
		current->pwd->i_count++;
	if (current->root)
		current->root->i_count++;
	if (current->executable)
		current->executable->i_count++;
	set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
	set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));
	p->state = TASK_RUNNING;	/* do this last, just in case */
	return last_pid;
}

int find_empty_process(void)
{
	int i;

	repeat:
		if ((++last_pid)<0) last_pid=1;
		for(i=0 ; i<NR_TASKS ; i++)
			if (task[i] && task[i]->pid == last_pid) goto repeat;
	for(i=1 ; i<NR_TASKS ; i++)
		if (!task[i])
			return i;
	return -EAGAIN;
}
