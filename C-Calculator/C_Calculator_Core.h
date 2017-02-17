#ifndef C_CALCULATOR_CORE
#define C_CALCULATOR_CORE

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "Util.h"

#define MAXLINE 100
#define MAXNUMBERLEN 100
#define _CCALC_DT long double
#define MAX_FACTORIAL 170

#define OPERANDS "+*/-^~"

typedef enum {
	FAT_EXPONENT = 1,
	FAT_FACTORIAL = 2,
	BAD_PARENTHESIS = 3,
	GEN_SYNTAX = 4,
	NEG_FACTORIAL = 5
} Error;

typedef enum {
	SUB_PRIORITY = 1,
	ADD_PRIORITY = 2,
	DIV_PRIORITY = 3,
	MULT_PRIORITY = 4,
	EXP_PRIORITY = 5,
	ROOT_PRIORITY = 6
} Priority;

struct expression {
	char operand;
	_CCALC_DT lefthand;
	_CCALC_DT righthand;
	int flagged;
	Priority priority;
};

Error __errFlag;
char * __errMsg;

//Main-executors
_CCALC_DT exec(char * command, _CCALC_DT * ans);
_CCALC_DT eval(char * expr, _CCALC_DT * eval);

//For evaluations
char getnextop(char * string, int fromIndex);
int isoperator(char op);
_CCALC_DT getnextnum(char * string, size_t fromindex);
int parenthesisAreConsistent(char * expr);
int hasParenthesis(char * expr);
int getnumlength(char * string, size_t fromindex);
_CCALC_DT ipow(_CCALC_DT base, _CCALC_DT exp);
_CCALC_DT factorial(_CCALC_DT num);
static char * expand(char * expr, _CCALC_DT * ans);

#endif
