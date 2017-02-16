#include "Util.h"

int getline(char * storage) {
	int i, c;

	i = 0;

	do {
		storage[i++] = c = getchar();
	} while (c != '\n' && i != MAXLINE - 1);

	if (i == MAXLINE - 1) {
		printf("\n\nError: Line too large to be parsed. Please try again\n\n");
		return 1;
	}

	storage[i] = '\0';
	return 0;
}

int moveup(long double val) {
	char * buffer = malloc(sizeof(char*) * 100);
	sprintf(buffer, "%lf", val);
	int toreturn = strlen(buffer);
	free(buffer);
	return toreturn;
}

static int strindex(char * s, char * t) {
	int i, j, k;

	for (i = 0; s[i] != '\0'; i++) {
		for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; j++, k++)
			;
		if (k > 0 && t[k] == '\0')
			return i;
	}

	return i;
}

char * eatSpaces(char * expr) {

	char * newstr = malloc(sizeof(char) * (_tcslen(expr)+1));
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

Priority getOperandPriority(char operand) {
	switch (operand) {
	case '*':
		return MULT_PRIORITY;
	case '/':
		return DIV_PRIORITY;
	case '+':
		return ADD_PRIORITY;
	case '-':
		return SUB_PRIORITY;
	case '^':
		return EXP_PRIORITY;
	default:
		return -1;
		break;
	}
}