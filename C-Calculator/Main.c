#include <stdio.h>
#include "calculator_core.h"

int32_t main()
{
	initialize_calculator();
	beginInputLoop();
	deinitialize_calculator();
	return 0;
}