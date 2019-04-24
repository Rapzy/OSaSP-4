#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "string.h"
#include "unistd.h"
#include "libgen.h"
#include "ctype.h"
#include "sys/time.h"

//1->2; 2->(3,4); 4->5; 3->6; 6->7; 7->8;
//1->(8,7) SIGUSR1; 8->(6,5) SIGUSR1; 5->(4,3,2) SIGUSR2; 2->1 SIGUSR2;
typedef struct {
        long tv_sec;
        long tv_usec;
    } timeval;

void waitChilds();
void createFile(int);
void deleteFiles();
int checkTree();
int readPid(int);
void getSignal(int);
long getTime();

char *progname;
int procNum, lastSender;

int main(int argc, char *argv[]) 
{
    argc += 0;
    progname = basename(argv[0]);
    signal(SIGINT, deleteFiles);
    pid_t pid = fork();
    if(pid == 0)//1
    {
        createFile(1);
        pid = fork();
        if(pid == 0)//2
        {
            createFile(2);
            pid = fork();
            if(pid==0)//4
            {
                createFile(4);
                pid = fork();
                if(pid==0)//5
                {
                    procNum = 5;
                    signal(SIGUSR1, getSignal);
                    setpgrp();
                    createFile(5);
                }
            }
            else if(pid > 0)
            {
                pid = fork();
                if(pid==0)//3
                {
                    createFile(3);
                }
                else
                {
                    pid = fork();
                    if (pid==0)//6
                    {
                        procNum = 6;
                        signal(SIGUSR1, getSignal);
                        setpgrp();
                        createFile(6);
                        pid = fork();
                        if(pid==0)//7
                        {
                            procNum = 7;
                            signal(SIGUSR1, getSignal);
                            setpgrp();
                            createFile(7);
                            pid = fork();
                            if(pid==0)//8
                            {
                                procNum = 8;
                                createFile(8);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            while(!checkTree());
            procNum = 1;
            lastSender = 1;
            killpg(readPid(7), SIGUSR1);
            printf("%d %d %d послал USR1 %ld.\n", procNum, getpid(), getppid(), getTime());
        }
        for(;;);
    }
    else if(pid > 0)
    {
        wait(NULL);
        deleteFiles();
    }
    return 0;
}
void getSignal(int sig)
{
    char *sigName = strdup(sys_signame[sig]);
    int i = 0;
    while (sigName[i] != '\0') {
        sigName[i] = toupper((unsigned char) sigName[i]);
        i++;
    }
    printf("%d %d %d получил %s %ld.\n", procNum, getpid(), getppid(), sigName, getTime());
}
int checkTree()
{
    char path[30];
    int pid;
    FILE *f;
    for (int i = 1; i <= 8; i++)
    {
        snprintf(path, sizeof(path), "/tmp/Lab4_%d.txt",i);
        if((f = fopen(path, "r")) == NULL)              
            return 0;
        else 
        {
            fscanf (f, "%d", &pid);
            if(pid > 0)
                continue;
        }
    }
    return 1;
}
long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (int)1e6 + tv.tv_usec;
}
int readPid(int num)
{
    char path[30];
    char pid[10];
    FILE *f;
    snprintf(path, sizeof(path), "/tmp/Lab4_%d.txt", num);
    if((f = fopen(path, "r")) == NULL)
    {
        fprintf(stderr, "%s: Couldn't open file %s.\n", progname, path);
		exit(1);
    }
    else
    {
        fgets(pid, sizeof(pid), f);
        return atoi(pid);
    }
}

void waitChilds()
{
    int wpid;
    while((wpid = wait(NULL)) > 0);
}
void createFile(int i)
{
    char path[30];
    snprintf(path, sizeof(path), "/tmp/Lab4_%d.txt", i);
    FILE *pidFile = fopen(path, "w");
    fprintf(pidFile, "%d", getpid());
    fclose(pidFile);
}

void deleteFiles()
{
    char path[30];
    for (int i = 1; i <= 8; i++)
    {
        kill(readPid(i),9);
        snprintf(path, sizeof(path), "/tmp/Lab4_%d.txt",i);
        remove(path);
    }
    exit(0);
}