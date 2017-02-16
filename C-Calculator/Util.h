#pragma once
#ifndef C_CALC_UTIL
#define C_CALC_UTIL

#include <string.h>
#include "C_Calculator_Core.h"

int getOperandPriority(char operand);
int getline(char * storage);
int strindex(char * s, char * t);
char * eatSpaces(char * exprs);
int moveup(long double val);
#endif