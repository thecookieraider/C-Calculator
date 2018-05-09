#ifndef CALCULATOR_IO_H
#define CALCULATOR_IO_H

#include <stdint.h>
#include <stdio.h>

#ifndef MAXLINE
#define MAXLINE 100
#endif

int32_t getline(char * storage)
{
	int32_t i, c;

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

#endif