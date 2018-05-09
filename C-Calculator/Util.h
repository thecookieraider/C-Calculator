#ifndef UTIL_H
#define UTIL_H
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int32_t parenthesisAreConsistent(char * expression)
{
	int32_t openCnt = 0;
	int32_t closedCnt = 0;

	for (size_t i = 0; i <= strlen(expression) + 1; i++) {
		if (openCnt < closedCnt) return 0;

		if (expression[i] == '\0' && (closedCnt == openCnt))
			return 1;
		else if (expression[i] == '\0' && (closedCnt != openCnt))
			return 0;


		if (expression[i] == '(')
			openCnt++;
		else if (expression[i] == ')')
			closedCnt++; 
	}

	return 0;
}

/* Remove this? */
//Need to use sprintf
#pragma warning(push)
#pragma warning(disable : 4996)
int32_t moveup(long double val) {
	char * buffer = (char *)malloc(sizeof(char*) * 100);
	sprintf(buffer, "%lf", val);
	int32_t toreturn = strlen(buffer);
	free(buffer);
	return toreturn;
}
#pragma warning(pop)

static int32_t strindex(char * s, char * t) {
	int32_t i, j, k;

	for (i = 0; s[i] != '\0'; i++) {
		for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
			;
		if (k > 0 && t[k] == '\0')
			return i;
	}

	return i;
}

char * eatSpaces(char * expr) {

	char * newstr = (char *)malloc(sizeof(char) * (strlen(expr)+1));
	char * begincache = newstr;
	while (*expr != '\0') {
		if (isspace(*expr)) {
			expr++;
			continue;
		}
		*newstr = *expr;
		newstr++;
		expr++;
	}
	*newstr = '\0';

	return begincache;
}

inline void * MallocOrDie(size_t size)
{
	void * toAlloc = malloc(size);

	if (toAlloc && size) return toAlloc;
	else {
		printf("Houston, we've got a problem. Memory couldn't be allocated by malloc");
		exit(1);
	}
}
#endif