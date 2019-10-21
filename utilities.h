#ifndef _UTILITIES_H_
#define _UTILITIES_H_

void exeSigInt(int sig);
char* rm_spaces(char* str);
char** tokenizeInput(char *input, int* numArgs);
//used for finding the pipe string position
int findStrPos(char **strs, int numStrs, char* toFind);

#endif
