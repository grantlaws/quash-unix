#ifndef _CMDLINE_H_
#define _CMDLINE_H_

struct Job {
    int pid;
    int jid;
    char* cmd;
};

void cd(char *dir);
void setPath(char **args);
void exeCommand(char *input, char **args, int numArgs);
void externalCommand(char **args);
void printJobs();
void exePipe(char **args, int numArgs);
void runInBackground(char *input, char **args, int numArgs);
void cmdline_exec();

#endif
