#include <stdio.h>
#include "cmdline.h"
#include "redirec.h"
int main(int argc, char** argv, char** envp)
{
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
		printf("No arguments to launch shell, ")
	}

}
