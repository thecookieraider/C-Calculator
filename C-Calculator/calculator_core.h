#ifndef CALCULATOR_CORE_H
#define CALCULATOR_CORE_H

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "hashmap.h"
#include "linked_list.h"

#ifndef MAX_ERROR_CODE
#define MAX_ERROR_CODE 256
#endif

struct expression_pack
{
	float previousEvaluation;
	char * expression;
	int32_t error;
};

struct expression_token
{
	char operand;
	float leftSide;
	float rightSide;
	int32_t priority;
	int8_t hasBeenComputed;
};

float multiply(float n1, float n2)
{
	return n1 * n2;
}

float divide(float n1, float n2)
{
	return n1 / n2;
}

float add(float n1, float n2)
{
	return n1 + n2;
}

float subtract(float n1, float n2)
{
	return n1 - n2;
}

/* 
Steps in parsing:
	-Ensure parenthesis are consistent
	-Process any functions present in the expression (build a hashtable of predefined functions such as log, exp, fact, etc. and then if the user defines one add it to this table)
	-If there are parenthesis, find the innermost expression, send it off to our evaluation function (which just goes thru an expression consistenting of numbers and operators and returns the result)
	 and work your way from in to out

	-You're finished when the expression in the parsing function is just one number

*/

static struct hashmap * errorMessages = NULL;
static struct func_hashmap * functions = NULL;
static int32_t calculatorIsInitialized = 0;

float getAndApplyFunction(char * name, float left, float right);
void insertFunction(char * name, Function f);

float parse(struct expression_pack * expression)
{
	return 0;
}

//This function will take any parenthesis-void expression and evaluate it
//with proper respect to order of operations. Note that the expression buffer
//passed to this function must be many times larger than the expression itself since any 
//integers present in the expression will be converted to floats and be represented as such
//in the buffer. The expression, when the function exits, will be completely empty, so if you wish to save
//the expression's data, do so before you call this function
#pragma warning(push)
#pragma warning(disable : 4996)
float evaluate(char * expression, size_t bufLength)
{
	expression = eatSpaces(expression);
	//Our intention in this function is to tokenize the expression
	//on each iteration and then solve token that has the highest precedence
	//and comes earliest in the expression
	struct expression_token tokens[MAXIMUM_EXPRESSION_LENGTH / 3];
	struct expression_token highestToken;
	//Keep some space to store our operand so we can pass it to our function hash table
	char operandFunctionName[2];

	//We'll need this when determining what token to solve
	int32_t highestPriorityScore = -1;
	size_t i, j;


	//Tokenizing is simple. Should be self explanatory.
	for (i = 0, j = 0; i < strlen(expression); j++) {
		if (isOperator(expression[i]) && i != 0 && i != '-') {
			tokens[j].leftSide = tokens[j - 1].rightSide;
			tokens[j].operand = expression[i];
			tokens[j].priority = getOperatorPriority(expression[i]);
			i++;
			tokens[j].rightSide = getNextNum(expression, i);
			i += getLengthOfNextNumber(expression, i);
		} else {
			tokens[j].leftSide = getNextNum(expression, i);
			i += getLengthOfNextNumber(expression, i);
			tokens[j].operand = expression[i];
			tokens[j].priority = getOperatorPriority(expression[i]);
			i++;
			tokens[j].rightSide = getNextNum(expression, i);
			i += getLengthOfNextNumber(expression, i);
		}
	}

	//Clear out the expression since we're going to rebuild
	memset(expression, 0, bufLength);

	//Save the start since we are going to advancing the pointer later on 
	char * startOfExpression = expression;
	char * toAdd = (char *)MallocOrDie(sizeof(char) * MAXIMUM_EXPRESSION_LENGTH);
	float result;
	size_t index;

	//Find the token that has the highest precedence
	for (i = 0; i < j; i++) {
		if (tokens[i].priority > highestPriorityScore) {
			highestPriorityScore = tokens[i].priority;
			highestToken = tokens[i];
			index = i;
		}
	}

	//Get the operand of the highest precedence token
	operandFunctionName[0] = highestToken.operand;
	operandFunctionName[1] = '\0';

	//Apply the function
	result = getAndApplyFunction(operandFunctionName, highestToken.leftSide, highestToken.rightSide);
	//Mark the token as the one that has been computed on this function run
	tokens[index].hasBeenComputed = 1;

	//If the compute token is not at the beginning, then augment the
	//token to the left of the computed token to have the computed
	//value as its right side
	if (index != 0) {
		tokens[index - 1].rightSide = result;
	}

	//If theres only one token then we are done since this was just
	//evaluated and there is nothing left to do now
	if (j == 1) {
		free(toAdd);
		return result;
	}

	//Piece the expression back together
	for(i = 0; i < j; i++){
		//If the beginning token is the one that was computed, just piece in the computed value
		if ((i == 0 && tokens[i].hasBeenComputed == 1)) sprintf(toAdd, "%f", result);
		//If the beginning token is not the one that was computed, piece in the entire token
		else if (i == 0 && tokens[i].hasBeenComputed != 1) sprintf(toAdd, "%f%c%f", tokens[i].leftSide, tokens[i].operand, tokens[i].rightSide);
		//If we're at a token that is not the computed token and is not the beginning token, just print the operand
		//and the right side of the token
		else if (i != 0 && tokens[i].hasBeenComputed != 1) sprintf(toAdd, "%c%f", tokens[i].operand, tokens[i].rightSide);
		//Anything else is undefined and should be skipped
		else continue;
		//Copy in the new piece we just built
		strcpy(expression, toAdd);
		//Move on for the next piece
		expression = expression + strlen(toAdd);
	}

	//Do it all again until we cant tokenize anymore
	return evaluate(startOfExpression, strlen(startOfExpression));
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4996)
int32_t insertError(int32_t code, char * message, char * usableBuffer)
{
	if (code > MAX_ERROR_CODE || code < 0) {
		return 0;
	} else {
		int32_t numberLength = 0;
		int32_t codeCopy = code;
		while (codeCopy > 1) { codeCopy /= 10; numberLength++; }

		hashmap_put(errorMessages, _itoa(code, usableBuffer, 10), message);
		memset(usableBuffer, ' ', numberLength * sizeof(char));
		return 1;
	}
}
#pragma warning(pop)

void insertFunction(char * name, Function functor)
{
	func_hashmap_put(functions, name, functor);
}

float getAndApplyFunction(char * name, float left, float right)
{
	Function functor = func_hashmap_get(functions, name)->data;
	return functor(left, right);
}

void initialize_calculator()
{
	if (errorMessages == NULL) errorMessages = new_hashmap();
	if (functions == NULL) functions = new_func_hashmap();

	char * buffer = (char *)MallocOrDie(sizeof(char)*MAX_ERROR_CODE);
	insertError(0, "Syntax Error. Parenthesis mismatch", buffer);
	insertError(1, "Syntax Error. Theres junk on the stack after grabbing the operator. You entered an obviously invalid expression", buffer);

	insertFunction("*", multiply);
	insertFunction("/", divide);
	insertFunction("+", add);
	insertFunction("-", subtract);

	free(buffer);
	calculatorIsInitialized = 1;
}

void deinitialize_calculator()
{
	hashmap_free(errorMessages);
	func_hashmap_free(functions);
}

#endif