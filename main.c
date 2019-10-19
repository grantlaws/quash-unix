#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "cmdline.h"
#include "redirec.h"
#include "utilities.h"

struct Job {
    int pid;
    int jid;
    char* cmd;
};

static struct Job jobs[100];
static int numJobs = 0;

static int numArgs = 0;

static char* user;
static char* directory;
static char* home;
static char* path;

void cd(char *dir) {
    if (dir == NULL) {
        chdir(getenv("HOME"));
        directory = getcwd(NULL, 1024);
    }
		else {
        if (chdir(dir) == -1) {
            printf(" %s: Directory does not exist!\n", strerror(errno));
        }
        directory = getcwd(NULL, 1024);
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
   		  printf("\nValue for '%s' is already set to '%s'.\nWould you like to reset this environment variable to '%s'?", newVar, getenv(newVar), value);
       	char *temp = readline(" (y/n): ");

        if (strcasecmp(temp, "y") == 0 || strcasecmp(temp, "yes") == 0) {
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
    for (int i = 0; i < numJobs; i++) {
        if (kill(jobs[i].jid, 0) == 0) {
            printf("\nJob ID: %d\nPID: %d\nCommand: %s\n", jobs[i].pid, jobs[i].jid, jobs[i].cmd);
        }
    }
}
// TODO
void exePipe(char **args, int numArgs) {}
// TODO
void runInBackground(char *input, char **args, int numArgs) {}

void exeCommand(char *input, char **args, int numArgs) {
    if (*args[numArgs-1] == '&') {
    	*args[numArgs-1] = 0;
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

int main(int argc, char** argv, char** envp)
{
	// if(argc == 1) //Command Line
	// {
	// 	cmdline_exec();
	// }
	// else if(argc > 1) //Read in from file
	// {
	// 	redirec_exec();
	// }
	// else
	// {
	// 	printf("This shouldn't happen.\n");
	// }

		rl_bind_key('\t', rl_complete);
    char* input, prompt[128];
    numJobs = 0;
		numArgs = 0;

		// infinite Loop
    while(1) {
    		user = getenv("USER");
        home = getenv("HOME");
        path = getenv("PATH");
        directory = getcwd(NULL, 1024);

        // Initialize Prompt
        snprintf(prompt, sizeof(prompt), "[%s:%s] > ", user, directory);

        // readline library allows to read in input and display prompt
        input = readline(prompt);

        // if input exists lets run it
        if (*input) {
            // remove spaces at the front and back of input
            input = rm_spaces(input);
						//if the input was not only spaces continue
            if (*input) {
                // add the current input to the history
                add_history(input);

                // tokenize input and place tokens into an array
                char **tokens = tokenize_input(input, &numArgs);

                // break out of while loop if first token is "exit" or "quit"
                if (strcmp("exit", tokens[0]) == 0 || strcmp("quit", tokens[0]) == 0) {
                    break;
                }

                exeCommand(input, tokens, numArgs);
            }
        }
				free(input);
        numArgs = 0;
    }


		printf("\n\nGOODBYE!\n\n");
    return 0;

}
