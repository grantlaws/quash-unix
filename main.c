#include "cmdline.h"
#include "redirec.h"

int main(int argc, char** argv, char** envp)
{
	if(argc < 2){
		cmdline_exec();
	} else{
		redirec_exec(argv);
	}
    return 0;

}
