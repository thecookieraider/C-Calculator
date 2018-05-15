#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifndef MAXLINE
#define MAXLINE 100  
#endif

#ifndef OPERATOR_STRING
#define OPERATOR_STRING "+-/*"
#endif

#ifndef MAXIMUM_EXPRESSION_LENGTH
#define MAXIMUM_EXPRESSION_LENGTH 4000
#endif

int32_t hasParenthesis(char * expression)
{
	for (size_t i = 0; i <= strlen(expression) + 1; i++) {
		if (expression[i] == '\0')
			return 0;

		if (expression[i] == '(' || expression[i] == ')')
			return 1;
	}

	return 0;
}

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

#pragma warning(push)
#pragma warning(disable : 4996)
int32_t getLengthOfNumber(float val) {
	char * buffer = (char *)malloc(sizeof(char*) * 100);
	sprintf(buffer, "%f", val);
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

inline void * CallocOrDie(size_t count, size_t size)
{
	void * toAlloc = calloc(count, size);

	if (toAlloc && size) return toAlloc;
	else {
		printf("Houston, we've got a problem. Memory couldn't be allocated by calloc");
		exit(1);
	}
}

void DieOnWindowsError(FILE * output)
{
	//Get the error message, if any.
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0)
		return; //No error message has been recorded

	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	fputs(messageBuffer, output);
	exit(1);
}

int32_t getline(char * storage)
{
	int32_t i;
	char c;

	i = 0;

	do {
		storage[i++] = c = getchar();
	} while (c != '\n' && i != MAXLINE - 1);

	if (i == MAXLINE - 1) {
		printf("\n\nError: Line too large to be parsed. Can only process %d characters at a time. Please try again\n\n", MAXLINE);
		return 1;
	}

	storage[i] = '\0';
	return 0;
}

float getNextNum(char * expression, size_t fromIndex)
{
	char * buffer = (char *)MallocOrDie(sizeof(char) * MAXLINE);

	int32_t i = fromIndex;
	int32_t j = 0;
	float sign = 1;

	while (!isdigit(expression[i]) && expression[i] != '\0' && expression[i] != '-' && expression[i] != '.')
		i++;

	if (expression[i] == '-') {
		sign = -1;
		i++;
	}


	for (j = 0; isdigit(expression[i]) || expression[i] == '.'; i++, j++)
		buffer[j] = expression[i];

	buffer[j] = '\0';

	float toReturn = (float)atof(buffer) * sign;

	free(buffer);
	
	return toReturn;
}

size_t getLengthOfNextNumber(char * expression, size_t fromIndex)
{
	int32_t i = fromIndex;
	int32_t j = 0;
	size_t size = 0;

	while (!isdigit(expression[i]) && expression[i] != '\0' && expression[i] != '-' && expression[i] != '.')
		i++;

	if (expression[i] == '-') {
		i++;
		size++;
	}


	while (isdigit(expression[i]) || expression[i] == '.') {
		size++;
		i++;
	}

	return size;
}

int32_t isOperator(char c)
{
	return strrchr(OPERATOR_STRING, c) != NULL;
}

char getNextOperator(char * string, int32_t fromIndex)
{
	if (fromIndex < 0)
		return -1;

	int32_t i;
	i = fromIndex;
	do {
		for (; (!isOperator(string[i])) && string[i] != '\0'; i++)
			;

		if (string[i] == '\0')
			return -1;

		if (isOperator(string[i])) {
			return string[i];
		}

	} while (1);
}

uint32_t getOperatorPriority(char operand)
{
	switch (operand) {
	case '*':
	case '/':
		return 1;
		break;
	case '+':
	case '-':
		return 0;
		break;
	}

	return -1;
}
#endif