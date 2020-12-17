#include <asm/segment.h>
#include <errno.h>
#include <string.h>

char kernalName[24];

int sys_iam(const char* name)
{
// name 's size can not above 23
	char tep[26];
	int i = 0;

	for(; i < 26; i++)
	{
		tep[i] = get_fs_byte(name+i);
		if(tep[i] == '\0')
			break;
	}
	
	if(i > 23)
	{
		errno = EINVAL;
		i = -1;
	}
	else
	{
		strcpy(kernalName, tep);
	}
	return i;
}

int sys_whoami(char* name, unsigned int size)
{
	int length = strlen(kernalName);
	int i = 0;	
	if(length <= size)
	{
		for( ; i < length; i++) 
			put_fs_byte(kernalName[i] ,name+i);	
		i++;	
	}		
	else
	{
		i = -1;
		errno = EINVAL;
	}
	return i;
}


