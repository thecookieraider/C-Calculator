#ifndef CALCULATOR_IO_H
#define CALCULATOR_IO_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <ctype.h>
#include "util.h"

#ifndef MAXLINE
#define MAXLINE 100
#endif

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

int32_t isOperator(char c)
{

}

float getNextNum(char * expression, size_t fromIndex)
{
	if (fromIndex < 0 || fromIndex >= strlen(expression) + 1)
		return 0;
	
	float value;
	int32_t i;
	int32_t j;

	int32_t sign = 1;

	char buffer[1024];

	i = fromIndex;


	while (!isdigit(expression[i]) && expression[i] != '\0' && expression[i] != '-' && expression[i] != '.')
		i++;

	if (expression[i] == '-') {
		sign = -1;
		i++;
	}


	for (j = 0; isdigit(expression[i]) || expression[i] == '.'; i++, j++)
		buffer[j] = expression[i];

	buffer[j] = '\0';

	value = atof(buffer) * sign;

	return value;
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
#endif