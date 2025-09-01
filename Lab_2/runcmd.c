#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Incorrect number of Arguements");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        execlp(argv[1], argv[1], argv[2], (char*)NULL);
            printf("Exec Failed");
                exit(1);
    }
    else if (pid > 0)
    {

        int status;
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        printf("Command Successfully Completed\n");
        else
        printf("Command Failed");
    }
    else
    {
        printf("Fork Failed\n");
        exit(1);
    }
}