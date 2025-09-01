#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();
    if (pid == 0)
    {
        printf("I am Child with pid %d\n", getpid());
        char *args[] = {"ls", "-l", NULL};
        execvp("ls", args);
        printf("If all goes well this will not be seen");

        exit(0);
    }
    else if (pid > 0)
    {
        printf("I am Parent Process %d\n", getpid());
        int status;
        wait(&status);
    }
    else
    {
        printf("Fork Failed\n");
        exit(1);
    }
}