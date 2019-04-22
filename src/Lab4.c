#include "stdio.h"
#include "signal.h"
#include "unistd.h"
 
//1->2; 2->(3,4); 4->5; 3->6; 6->7; 7->8;
int main() 
{
    printf("Process N: %d | pid: %d\n",0,getpid());
    pid_t pid = fork();
    printf("PID: %d\n",pid);
    if(pid == 0)//1
    {
        printf("Process N: %d | pid: %d\n",1,getpid());
        pid = fork();
        printf("PID1: %d\n",pid);
        if(pid == 0)//2
        {
            printf("Process N: %d | pid: %d\n",2,getpid());
            pid = fork();
            if(pid==0)//4
            {
                printf("Process N: %d | pid: %d\n",4,getpid());
                pid = fork();
                if(pid==0)//5
                {
                    printf("Process N: %d | pid: %d\n",5,getpid());
                }
            }
            else if(pid > 0)
            {
                pid = fork();
                if(pid==0)//3
                {
                    printf("Process N: %d | pid: %d\n",3,getpid());
                }
                else
                {
                    pid = fork();
                    if (pid==0)//6
                    {
                        printf("Process N: %d | pid: %d\n",6,getpid());
                        pid = fork();
                        if(pid==0)//7
                        {
                            printf("Process N: %d | pid: %d\n",7,getpid());
                            pid = fork();
                            if(pid==0)//8
                            {
                                printf("Process N: %d | pid: %d\n",8,getpid());
                            }
                        }
                    }
                }
            }
        }
        exit(0);
    }
    else if(pid > 0)
        waitpid(pid);
    return 0;
}