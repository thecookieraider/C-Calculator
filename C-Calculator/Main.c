#include <stdio.h>
#include <Windows.h>
#include "calculator_core.h"

int32_t main()
{
	initialize_calculator();
	char * e = "-12 + 2";
	char * expression = (char *)MallocOrDie(sizeof(char) * MAXIMUM_EXPRESSION_LENGTH);
	strcpy(expression, e);

	float result = evaluate(expression, sizeof(char) * MAXIMUM_EXPRESSION_LENGTH);
	getchar();
}