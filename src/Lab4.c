#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "string.h"
#include "unistd.h"
 
//1->2; 2->(3,4); 4->5; 3->6; 6->7; 7->8;
//1->(8,7) SIGUSR1; 8->(6,5) SIGUSR1; 5->(4,3,2) SIGUSR2; 2->1 SIGUSR2;
void waitChilds();
void createFile(int);
void deleteFiles();

int main() 
{
    signal(SIGINT, deleteFiles);
    printf("Process N: %d | pid: %d\n",0,getpid());
    pid_t pid = fork();
    if(pid == 0)//1
    {
        createFile(1);
        printf("Process N: %d | pid: %d\n",1,getpid());
        pid = fork();
        if(pid == 0)//2
        {
            createFile(2);
            printf("Process N: %d | pid: %d\n",2,getpid());
            pid = fork();
            if(pid==0)//4
            {
                createFile(4);
                printf("Process N: %d | pid: %d\n",4,getpid());
                pid = fork();
                if(pid==0)//5
                {
                    createFile(5);
                    printf("Process N: %d | pid: %d\n",5,getpid());
                }
                else
                    waitChilds();
            }
            else if(pid > 0)
            {
                pid = fork();
                if(pid==0)//3
                {
                    createFile(3);
                    printf("Process N: %d | pid: %d\n",3,getpid());
                }
                else
                {
                    pid = fork();
                    if (pid==0)//6
                    {
                        createFile(6);
                        printf("Process N: %d | pid: %d\n",6,getpid());
                        pid = fork();
                        if(pid==0)//7
                        {
                            createFile(7);
                            printf("Process N: %d | pid: %d\n",7,getpid());
                            pid = fork();
                            if(pid==0)//8
                            {
                                createFile(8);
                                printf("Process N: %d | pid: %d\n",8,getpid());
                            }
                        }
                        else 
                            waitChilds();
                    }
                    else
                        waitChilds();
                    waitChilds();
                }
                waitChilds();
            }
        }
        else
            waitChilds();
        //for(;;);
    }
    else if(pid > 0)
        wait(NULL);
    deleteFiles();
    return 0;
}
void waitChilds()
{
    int wpid;
    while((wpid = wait(NULL)) > 0)
    {
        //printf("WAIT %d\n", wpid);
    }
}
void createFile(int i)
{
    char path[30];
    snprintf(path, sizeof(path), "/tmp/Lab4/%d.txt", i);
    FILE *pidFile = fopen(path, "w");
    fprintf(pidFile, "%d", getpid());
    fclose(pidFile);
}

void deleteFiles()
{
    char path[30];
    for (size_t i = 1; i <= 8; i++)
    {
        snprintf(path, sizeof(path), "/tmp/Lab4/%d.txt",i);
        remove(path);
    }
    
}