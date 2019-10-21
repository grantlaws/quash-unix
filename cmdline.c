#include "cmdline.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>

static struct Job jobs[100];
static int *numJobs = 0;

static int numArgs = 0;
static char* user;
static char* directory;
static char* home;
static char* path;
static char* input;
static size_t inputSize = 32;

void cmdline_exec()
{
	numJobs = mmap(NULL, sizeof *numJobs, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *numJobs = 0;
	numArgs = 0;

    // look if SIGINT signal is received, set handler
    signal(SIGINT, exeSigInt);

    input = (char *)malloc(inputSize * sizeof(char));
     if( input == NULL)
     {
         perror("Unable to allocate buffer");
         exit(1);
     }

	// infinite Loop
    while(1) {
		//according to standard 7.20.4.5, getenv can overwrite itself and shouldn't be modified by program
		user = getenv("USER");
		home = getenv("HOME");
		path = getenv("PATH");
		
		//Pointer created by getcwd has to be freed	
		if(directory != NULL){
			free(directory);
			directory = getcwd(NULL, 1024);
		} else {
			directory = getcwd(NULL, 1024);
		}

		printf("[%s:%s] > ", user, directory);
		getline(&input, &inputSize, stdin);
		// if input exists lets run it
		if (*input) {
			// remove spaces at the front and back of input
			input = rm_spaces(input);
			
			//if the input was not only spaces continue
			if (*input) {
				// tokenize input and place tokens into an array
				char **tokens = tokenizeInput(input, &numArgs);

				// break out of while loop if first token is "exit" or "quit"
				if (strcmp("exit", tokens[0]) == 0 || strcmp("quit", tokens[0]) == 0) {
					if(directory != NULL){
						free(directory);
					}
					if(input != NULL){
						free(input);
					}
					if(tokens != NULL){
						free(tokens);
					}
					break;
				}

				exeCommand(input, tokens, numArgs);
				free(tokens);
			}
		}
		numArgs = 0;
    }


	printf("\n\nGOODBYE!\n\n");
}

void cd(char *dir) {
    if (dir == NULL) {
        chdir(getenv("HOME"));
		if(directory != NULL){
			free(directory);
			directory = getcwd(NULL, 1024);
		} else {
			directory = getcwd(NULL, 1024);
		}
    } else {
        if (chdir(dir) == -1) {
        	printf(" %s: Directory does not exist!\n", strerror(errno));
        }
	if(directory != NULL){
			free(directory);
			directory = getcwd(NULL, 1024);
		} else {
			directory = getcwd(NULL, 1024);
		}
    }
}

void setPath(char **args) {
	//check that correct amount of args have been passed
	if(args[1] && args[2]){
    //get the variable to be set and the value to set it to
    char *newVar = args[1];
    char *value = args[2];
    setenv(newVar, value, 0);

     if (strcmp(value, getenv(newVar)) == 0) {
        printf("\nNew environment variable '%s' was set to the value '%s' successfully.\n\n", newVar, getenv(newVar));
     } else {
   	 printf("\nValue for '%s' is already set to '%s'.\nWould you like to reset this environment variable to '%s'?\n> ", newVar, getenv(newVar), value);
       	printf("(y/n): ");

	//THE BELOW DOES NOT WORK
        getline(&input, &inputSize, stdin);
        if (strcasecmp(input, "y") == 0 || strcasecmp(input, "yes") == 0) {
          	setenv(newVar, value, 1);
           	printf("Environment variable '%s' was set to the value '%s' successfully.\n\n", newVar, getenv(newVar));
        } else {
	           	printf("%s", "\n");
       	}
     }
	} else{
		printf("Insuffcient arguements for set.\n");
	}
}

void externalCommand(char **args) {
    pid_t pid = fork();
    int status;

    if (pid == 0) {
        if (execlp(args[0], args[0], args[1], args[2], args[3], args[4], args[5], NULL) < 0) {
            fprintf(stderr, "\nInvalid command.\n\n");
            exit(-1);
        }
    } else {
    	waitpid(pid, &status, 0);

    	if (status == 1) {
            fprintf(stderr, "%s", "FAIL");
    	}
    }
}

void printJobs() {
    for (int i = 0; i < *numJobs; i++) {
        if (kill(jobs[i].jid, 0) == 0) {
            printf("\nJob ID: %d\nPID: %d\nCommand: %s\n", jobs[i].pid, jobs[i].jid, jobs[i].cmd);
        }
    }
}

void exeCommand(char *input, char **args, int numArgs) {
    if (*args[0] == '&') {
    	*args[0] = 0;
    	runInBackground(input, args, numArgs);
    }
		else if (strcmp("set", args[0]) == 0) {
     	setPath(args);
    }
		else if (strcmp("cd", args[0]) == 0) {
    	cd(args[1]);
  	}
		else if (strcmp("jobs", args[0]) == 0) {
    	printJobs();
    }
		else if (findStrPos(args, numArgs, "|") > -1) {
    	exePipe(args, numArgs);
    }
		else {
    	externalCommand(args);
    }
}

void exePipe(char **args, int numArgs) {
  int pfd[2];
  char *input = strdup(*args);
  char *arg1 = strtok(input, "|");
  char *arg2 = strtok(NULL, "\n");

  int pid;

  pid_t pid_1, pid_2;

  pid_1 = fork();
  if (pid_1 == 0) {
    dup2(pfd[1], STDOUT_FILENO);
    close(pfd[0]);
    int n = 0;
    char **tmp = tokenizeInput(arg1, &n);
    exeCommand(rm_spaces(arg1), tmp, n);
    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0) {
    dup2(pfd[0], STDIN_FILENO);
    close(pfd[1]);
    int n = 0;
    char **tmp = tokenizeInput(arg1, &n);
    exeCommand(rm_spaces(arg2), tmp, n);
  }

  close(pfd[0]);
  close(pfd[1]);
}

void runInBackground(char *input, char **args, int numArgs) {
  pid_t pid, sid;

  pid = fork();
  if (pid == 0) {
      sid = setsid();

      if (sid < 0) {
          printf("\nFailed to create child process.\n\n");
          exit(EXIT_FAILURE);
      }

      printf("\n[%d] %d is running in the background\n\n", getpid(), *numJobs);

      exeCommand(input, args, numArgs);

      printf("\n[%d] done\n\n", getpid());

      kill(getpid(), -9);
      exit(0);
  } else {
    struct Job newJob = {
      .pid = pid,
      .jid = *numJobs,
      .cmd = input
    };

      int status;

    jobs[*numJobs] = newJob;
    *numJobs = *numJobs + 1;

      while(waitpid(pid, NULL, WEXITED|WNOHANG) > 0) {
      }
  }
}

