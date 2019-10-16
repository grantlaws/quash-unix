#include <stdio.h>
#include "cmdline.h"
#include "redirec.h"
#include <string.h>

int main(int argc, char** argv, char** envp)
{
    if(argc > 1 && strcmp(argv[1],"env") == 0){
        for(int i = 0; i < strlen(*envp); i++)
        {
            printf("%s\n", envp[i]);
        }
    }
	if(argc == 1) //Command Line
	{
		cmdline_exec();
	}
	else if(argc > 1) //Read in from file
	{
		redirec_exec();
	}
	else
	{
		printf("This shouldn't happen.\n");
	}

}
