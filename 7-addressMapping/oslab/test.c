#define __LIBRARY__

#include <unistd.h>

_syscall3(long int, shmget, int, key, unsigned int, size, int, shmflg);
_syscall3(void *, shmat, long int, shmid, const void *, shmaddr, int, shmflg);


int main()
{
    long int shmID = shmget(1, 10*sizeof(int), 0);
    int *buffer = (int *)shmat(shmID, 0,0);
    int i =0;
    printf("\nphysical address = %d,logical address = %d\n",shmID, buffer);
    
    buffer[1] = 10;
    printf("buffer[1] = %d\n",buffer[1]);
    for(;i<10;i++)
    {
        scanf("%d",&buffer[i]);
        printf("buffer[%d] = %d\n",i, buffer[i]);
    }
    return 0;
}
