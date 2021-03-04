/*
 *  linux/fs/blosk_dev.c
 *
 *  (C) 1991  Linus Torvalds
 */

#include <errno.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>

#include <stdarg.h>
#include <stddef.h>

#include <string.h>
#define name_to_str(name) (#name)
int block_write(int dev, long *pos, char *buf, int count)
{
	int block = *pos >> BLOCK_SIZE_BITS;
	int offset = *pos & (BLOCK_SIZE - 1);
	int chars;
	int written = 0;
	struct buffer_head *bh;
	register char *p;

	while (count > 0)
	{
		chars = BLOCK_SIZE - offset;
		if (chars > count)
			chars = count;
		if (chars == BLOCK_SIZE)
			bh = getblk(dev, block);
		else
			bh = breada(dev, block, block + 1, block + 2, -1);
		block++;
		if (!bh)
			return written ? written : -EIO;
		p = offset + bh->b_data;
		offset = 0;
		*pos += chars;
		written += chars;
		count -= chars;
		while (chars-- > 0)
			*(p++) = get_fs_byte(buf++);
		bh->b_dirt = 1;
		brelse(bh);
	}
	return written;
}

int block_read(int dev, unsigned long *pos, char *buf, int count)
{
	int block = *pos >> BLOCK_SIZE_BITS;
	int offset = *pos & (BLOCK_SIZE - 1);
	int chars;
	int read = 0;
	struct buffer_head *bh;
	register char *p;

	while (count > 0)
	{
		chars = BLOCK_SIZE - offset;
		if (chars > count)
			chars = count;
		if (!(bh = breada(dev, block, block + 1, block + 2, -1)))
			return read ? read : -EIO;
		block++;
		p = offset + bh->b_data;
		offset = 0;
		*pos += chars;
		read += chars;
		count -= chars;
		while (chars-- > 0)
			put_fs_byte(*(p++), buf++);
		brelse(bh);
	}
	return read;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;
	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}

/* 
 * proc_read 要把当前系统状态写到 buf 里面
 * buf 里面写 进程的 pid 和 state
 * sprintf 用来把 long , int 类型转换为 char
 * 
 */

int proc_read(int dev, off_t *pos, char *buf, int count)
{

	struct task_struct **p;
	char procBuffer[1025];

	int lenOfProcBuffer = 0;

	int endOfBuf = 0;
	printk("proc read start ...\n");

	// 0 代表读取 psinfo
	if (dev == 0)
	{

		char *field = "pid\t\tstate\n\0";
		lenOfProcBuffer = sprintf(procBuffer, field);

		// 读取进程状态到 procBuffer
		for (p = &FIRST_TASK; p <= &LAST_TASK; p++)
		{
			if ((*p) != NULL)
			{
				lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%ld\t\t%ld\n", (*p)->pid, (*p)->state);
			}
		}
		// 从内核态空间写数据到用户态
		// 只要 cat 命令的 buf 没到上限
		// 以及 procBuffer 没到上限
		while (endOfBuf < count && procBuffer[(*pos)] != '\0')
		{
			put_fs_byte(procBuffer[(*pos)], buf + endOfBuf);
			endOfBuf++;
			(*pos)++;
		}
	}
	// 1 代表读取 hdinfo
	if(dev==1){
		// 写字段到 procBuffer
		char *field = "filed\t\tvalue\n";
		lenOfProcBuffer = sprintf(procBuffer, field);
		// 获取超级块
		struct super_block * sb;
		sb =get_super(dev);

		unsigned short total_logic_blocks = sb->s_nzones;
		unsigned short total_inodes = sb->s_ninodes;
		unsigned short total_inodes_bitmap = sb->s_imap_blocks;
		unsigned short total_logic_blocks_bitmap = sb->s_zmap_blocks;
		unsigned short first_logic_block_num = sb->s_firstdatazone;

		// 写信息到 procBuffer
		lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%s : %u\n", name_to_str(total_logic_blocks), total_logic_blocks);
		lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%s : %u\n",name_to_str(total_inodes),total_inodes);
		lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%s : %u\n",name_to_str( total_inodes_bitmap),total_inodes_bitmap);
		lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%s : %u\n", name_to_str(total_logic_blocks_bitmap),total_logic_blocks_bitmap);
		lenOfProcBuffer += sprintf(procBuffer + lenOfProcBuffer, "%s : %u\n", name_to_str(first_logic_block_num),first_logic_block_num);
	
		// 从内核态空间写数据到用户态
		// 只要 cat 命令的 buf 没到上限
		// 以及 procBuffer 没到上限
		while (endOfBuf < count && procBuffer[(*pos)] != '\0')
		{
			put_fs_byte(procBuffer[(*pos)], buf + endOfBuf);
			endOfBuf++;
			(*pos)++;
		}
	}
	return endOfBuf;
}
