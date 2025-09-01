#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BG_PROCS 64
pid_t bg_procs[MAX_BG_PROCS];
int bg_pgid = -1;
pid_t fg_pid = 0;

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
	char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
	char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
	int i, tokenIndex = 0, tokenNo = 0;

	for (i = 0; i < strlen(line); i++)
	{

		char readChar = line[i];

		if (readChar == ' ' || readChar == '\n' || readChar == '\t')
		{
			token[tokenIndex] = '\0';
			if (tokenIndex != 0)
			{
				tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0;
			}
		}
		else
		{
			token[tokenIndex++] = readChar;
		}
	}

	free(token);
	tokens[tokenNo] = NULL;
	return tokens;
}

void cleanUp(char **tokens)
{
	for (int i = 0; tokens[i] != NULL; i++)
	{
		free(tokens[i]);
	}
	free(tokens);
}

void SIGINT_Handler(int sig)
{
	if (fg_pid > 0)
	{
		// If Foreground Job Running then Kill It
		kill(fg_pid, SIGINT);
		// Print new line in Shell
		write(STDOUT_FILENO, "\n", 1);
	}
}

int main(int argc, char *argv[])
{
	char line[MAX_INPUT_SIZE];
	char **tokens;
	int i;
	for (i = 0; i < MAX_BG_PROCS; i++)
		bg_procs[i] = -1;

	// Registering Interrupt Handler
	struct sigaction sa;
	// Assign Handler
	sa.sa_handler = SIGINT_Handler;
	// Allow different system calls during Interrupt although it wont matter
	sigemptyset(&sa.sa_mask);
	// Give shell immediate control
	sa.sa_flags = 0;
	// Register Handler
	sigaction(SIGINT, &sa, NULL);

	while (1)
	{
		// Reap Background Children
		int status;
		pid_t done;
		done = waitpid(-1, &status, WNOHANG);
		while (done > 0)
		{
			// Remove PID from bg_procs
			for (int i = 0; i < MAX_BG_PROCS; i++)
			{
				if (bg_procs[i] == done)
				{
					bg_procs[i] = -1;
					break;
				}
			}

			printf("\n[Background Process %d Finished]\n", done);

			// Check for next finished child
			done = waitpid(-1, &status, WNOHANG);
		}
		/* BEGIN: TAKING INPUT */

		// Initialize boolean to check for background process
		int is_background = 0;
		bzero(line, sizeof(line));

		// Add Current Working Directory
		char cwd[256];
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf("%s$ ", cwd);
		}
		else
			printf("$ ");

		scanf("%[^\n]", line);
		getchar();

		// printf("Command entered: %s (remove this debug output later)\n", line);
		/* END: TAKING INPUT */

		line[strlen(line)] = '\n'; // terminate with new line
		tokens = tokenize(line);

		// Checking Null Inputs
		if (tokens[0] == NULL)
		{
			free(tokens);
			continue;
		}

		// Cheking background process
		for (i = 0; tokens[i] != NULL; i++)
		{
			if (strcmp(tokens[i], "&") == 0 && tokens[i + 1] == NULL)
			{
				is_background = 1;
				tokens[i] = NULL;
			}
		}

		// Checking and Handling Directory Change
		if (strcmp(tokens[0], "cd") == 0)
		{
			if (tokens[1] == NULL || tokens[2] != NULL)
			{
				fprintf(stderr, "Usage: cd <directory>\n");
			}
			else
			{
				if (chdir(tokens[1]) != 0)
				{
					perror("cd");
				}
			}
			continue;
		}

		// Checking and Handling Exit
		if (strcmp(tokens[0], "exit") == 0 && tokens[1] == NULL)
		{
			for (i = 0; i < MAX_BG_PROCS; i++)
			{
				if (bg_procs[i] != -1)
				{
					kill(bg_procs[i], SIGTERM);
					waitpid(bg_procs[i], NULL, 0);
					bg_procs[i] = -1;
				}
			}
			cleanUp(tokens);
			printf("Exiting shell...\n");
			exit(0);
		}

		pid_t pid = fork();
		if (pid < 0)
		{
			printf("Fork Failed");
		}
		else if (pid == 0)
		{
			// Setting Background Process Group ID to first Background Process PID
			if (is_background)
			{
				if (bg_pgid == -1)
				{
					bg_pgid = getpid();
				}
				setpgid(0, bg_pgid);
			}
			execvp(tokens[0], tokens);
			perror("myshell");
			exit(1);
		}
		else
		{
			// wait(&status);
			// If Foreground Process
			if (!is_background)
			{
				fg_pid = pid;
				waitpid(pid, &status, 0);
				fg_pid = 0;
			}
			else
			{
				for (i = 0; i < MAX_BG_PROCS; i++)
				{
					if (bg_procs[i] == -1)
					{
						bg_procs[i] = pid;
						break;
					}
				}
			}

			if (!is_background && WEXITSTATUS(status))

			{
				printf("\nEXITSTATUS: %d\n", WEXITSTATUS(status));
			}
			// Reap Background Children
			pid_t done;
			done = waitpid(-1, &status, WNOHANG);
			while (done > 0)
			{
				// Remove PID from bg_procs
				for (int i = 0; i < MAX_BG_PROCS; i++)
				{
					if (bg_procs[i] == done)
					{
						bg_procs[i] = -1;
						break;
					}
				}

				printf("\n[Background Process %d Finished]\n", done);

				// Check for next finished child
				done = waitpid(-1, &status, WNOHANG);
			}
		}

		// do whatever you want with the commands, here we just print them
		// for(i=0;tokens[i]!=NULL;i++){
		// 	printf("found token %s (remove this debug output later)\n", tokens[i]);
		// }

		// Freeing the allocated memory
		cleanUp(tokens);
	}
	return 0;
}
