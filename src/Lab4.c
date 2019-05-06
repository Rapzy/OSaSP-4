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
int sigCount = 0;

int main(int argc, char *argv[]) 
{
    argc += 0;
    progname = basename(argv[0]);
    signal(SIGINT, deleteFiles);
    pid_t pid = fork();
    //create 1 process
    if(pid == 0)
    {
        procNum = 1;
        createFile(1);
        setpgrp();
        signal(SIGUSR2, getSignal);
        pid = fork();
        //create 2 process
        if(pid == 0)
        {
            procNum = 2;
            createFile(2);
            setpgrp();
            pid = fork();
            //create 4 process
            if(pid==0)
            {
                createFile(4);
                pid = fork();
                //create 5 process
                if(pid==0)
                {
                    procNum = 5;
                    signal(SIGUSR1, getSignal);
                    createFile(5);
                    setpgrp(); //Set 5 gpid = pid5
                }
            }
            else if(pid > 0)
            {
                pid = fork();
                //create 3 process
                if(pid==0)
                {
                    procNum = 3;
                    createFile(3);
                }
                else
                {
                    pid = fork();
                    //create 6 process
                    if (pid==0)
                    {
                        procNum = 6;
                        signal(SIGUSR1, getSignal);
                        createFile(6);
                        pid = fork();
                        //create 7 process
                        if(pid==0)
                        {
                            procNum = 7;
                            signal(SIGUSR1, getSignal);
                            setpgrp(); //Set 7 gpid = pid7
                            createFile(7);
                            pid = fork();
                            //create 8 process
                            if(pid==0)
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
            setpgid(readPid(6), readPid(5)); //Set 6 gpid = pid5
            // for (int i = 1; i <= 8; i++)
            // {
            //     printf("%d %d\n", i,getpgid(readPid(i)));
            // }
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
    pid_t pid = getpid();
    printf("%d %d %d получил %s %ld.\n", procNum, pid, getppid(), sigName, getTime());
    switch (procNum)
    {
    case 1:
        if(sigCount == 100)
        {
            for (int i = 1; i <= 8; i++)
            {
                kill(readPid(i), SIGTERM);
            }
            
        }
        sigCount++;
        killpg(readPid(7), SIGUSR1);
        printf("%d %d %d послал USR1 %ld.\n", procNum, pid, getppid(), getTime());
        break;
    case 2:
        killpg(readPid(1), SIGUSR2);
        printf("%d %d %d послал USR2 %ld.\n", procNum, pid, getppid(), getTime());
        break;
    case 5:
        killpg(readPid(2), SIGUSR2);
        printf("%d %d %d послал USR2 %ld.\n", procNum, pid, getppid(), getTime());
        break;
    case 8:
        killpg(readPid(5), SIGUSR1);
        printf("%d %d %d послал USR1 %ld.\n", procNum, pid, getppid(), getTime());
    default:
        break;
    }
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