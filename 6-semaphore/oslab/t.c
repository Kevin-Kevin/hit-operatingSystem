#include <stdio.h>
#include <stdlib.h>

/* sem_t sem[5] = 
{
    {0, NULL, NULL, 'NULL', 0},
    {0, NULL, NULL, 'NULL', 0},
    {0, NULL, NULL, 'NULL', 0},
    {0, NULL, NULL, 'NULL', 0},
    {0, NULL, NULL, 'NULL', 0}
}; */

sem_t sem[5] = 
{
    {0, NULL, NULL, "NULL", 0},
    {0, NULL, NULL, "NULL", 0},
    {0, NULL, NULL, "NULL", 0},
    {0, NULL, NULL, "NULL", 0},
    {0, NULL, NULL, "NULL", 0}
    
};

int main()
{
    int i = 0;
for(i = 0; i<5; i++)
    {
        printf("%d : [ %s ] value = %d\n", &sem[i], sem[i].name, sem[i].value);
    }
    return 0;
}