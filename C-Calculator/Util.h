#pragma once
#ifndef C_CALC_UTIL
#define C_CALC_UTIL

#include <string.h>
#include "C_Calculator_Core.h"

int32_t getOperandPriority(char operand);
int32_t getline(char * storage);
int32_t strindex(char * s, char * t);
char * eatSpaces(char * exprs);
int32_t moveup(long double val);
#endif