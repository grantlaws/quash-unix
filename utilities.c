#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* rm_spaces(char* str) {
    char *endChar;

		//move pointer to first character in string
    while (isspace(*str))
		str++;

		//return if only spaces
    if (*str == 0)
		return str;

		//set endChar to the last char in string
    endChar = str + strlen(str) - 1;

		//move endChar to the last non space char
    while (endChar > str && isspace(*endChar))
		endChar--;

		//set last end char to null
    *(endChar + 1) = 0;

		//returns string with no spaces at the front or back
    return str;
}

char** tokenize_input(char *input, int* numArgs) {
	char **ret = NULL;
	char *temp = strtok(input, " ");
	int i, numSpaces = 0;

	while(temp) {
		ret = realloc(ret, sizeof(char*) * ++numSpaces);

		if (ret == NULL) {
			printf("\nMemory allocation failed for command '%s'\n\n", input);
			return ret;
		}

		// If token is a string, put entire string in token and remove quotes
		if (temp[0] == '"') {
			temp++;
			sprintf(temp, "%s%s%s", temp, " ", strtok(NULL, "\""));
		}

		ret[numSpaces-1] = temp;
		*numArgs = (*numArgs) + 1;

		temp = strtok(NULL, " =");
	}

	ret = realloc(ret, sizeof(char*) * (numSpaces+1));
	ret[numSpaces] = 0;

	return ret;
}

int findStrPos(char **strs, int numStrs, char* toFind) {
    int i;

    for (i = 0; i < numStrs; i++) {
        if (strcmp(strs[i], toFind) == 0) {
            return i;
        }
    }

    return -1;
}
