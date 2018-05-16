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

#ifndef PREV_EVAL_SHORTCODE
#define PREV_EVAL_SHORTCODE "ans"
#endif

struct expression_pack
{
	float previousEvaluation;
	size_t expressionBufferSize;
	char * expression;
	char * error;
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

void insertFunction(char * name, Function f);
float getAndApplyFunction(char * name, float left, float right);
float evaluate(char * expression, size_t bufLength);

#pragma warning(push)
#pragma warning(disable : 4996)
float parse(struct expression_pack * expression)
{
	expression->expression = eatSpaces(expression->expression, expression->expressionBufferSize);

	//Sanity checks before we process this input
	if (!calculatorIsInitialized) {
		printf("System Error. Calculator system has not been initialized and it could not parse the given expression");
		expression->previousEvaluation = 0;
		return 0;
	} else if (expressionHasConsequetiveOperators(expression->expression)) {
		expression->error = "2";
		return 0;
	} else if (!parenthesisAreConsistent(expression->expression)) {
		expression->error = "1";
		return 0;
	}

	//Expand the ans variable
	if (strstr(expression->expression, PREV_EVAL_SHORTCODE) != NULL) {
		size_t ansLength = getLengthOfNumber(expression->previousEvaluation);
		int32_t i = 0;

		char * ansBuffer = (char *)MallocOrDie(sizeof(char)*MAX_NUMBER_LENGTH);
		char * newExpression = (char *)MallocOrDie(expression->expressionBufferSize);
		char * expresssionBegin = newExpression;
		sprintf(ansBuffer, "%f", expression->previousEvaluation);

		while(*expression->expression != '\0') {
			if (strstr(expression->expression, PREV_EVAL_SHORTCODE) == expression->expression) {
				strcpy(&newExpression[i], ansBuffer);
				i += ansLength;
				expression->expression += strlen(PREV_EVAL_SHORTCODE);
			} else {
				newExpression[i++] = *expression->expression++;
			}
		}


		newExpression[i] = '\0';
		memset(expression->expression, 0, expression->expressionBufferSize);
		memcpy(expression->expression, newExpression, expression->expressionBufferSize);
		free(ansBuffer);
		free(newExpression);
	}
	if (hasParenthesis(expression->expression)) {
		char * buffer = (char *)MallocOrDie(expression->expressionBufferSize);
		char * resultBuffer = (char *)MallocOrDie(sizeof(char) * MAX_NUMBER_LENGTH);
		float result;

		while (strchr(expression->expression, '(')) {
			ptrdiff_t i, j;
			ptrdiff_t lastOpeningParenthesisIndex = strrchr(expression->expression, '(') - expression->expression;
			ptrdiff_t closingParenthesisIndex = strchr(expression->expression + lastOpeningParenthesisIndex, ')') - expression->expression;
			for (i = lastOpeningParenthesisIndex + 1, j = 0; i < closingParenthesisIndex; i++, j++) {
				buffer[j] = expression->expression[i];
			}
			
			buffer[j] = '\0';

			result = evaluate(buffer, expression->expressionBufferSize);
			sprintf(resultBuffer, "%f", result);
			memset(buffer, 0, expression->expressionBufferSize);

			size_t i2, j2;
			for (i2 = 0, j2 = 0; j2 < strlen(expression->expression);) {
				if (i2 == lastOpeningParenthesisIndex) {
					strcpy(&buffer[i2], resultBuffer);
					i2 += getLengthOfNumber(result);
					j2 += (closingParenthesisIndex - lastOpeningParenthesisIndex) + 1;
				} else {
					buffer[i2++] = expression->expression[j2++];
				}
			}

			memcpy(expression->expression, buffer, expression->expressionBufferSize);
			memset(buffer, 0, expression->expressionBufferSize);
		}

		expression->previousEvaluation = evaluate(expression->expression, expression->expressionBufferSize);
		return expression->previousEvaluation;
	} else {
		expression->previousEvaluation = evaluate(expression->expression, expression->expressionBufferSize);
		return expression->previousEvaluation;
	}
	
	expression->previousEvaluation = evaluate(expression->expression, expression->expressionBufferSize);
	return expression->previousEvaluation;
}
#pragma warning(pop)

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
	if (getLengthOfNextNumber(expression, 0) == strlen(expression)) return getNextNum(expression, 0);
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
int32_t insertError(char * code, char * message)
{
	if (atoi(code) > MAX_ERROR_CODE || atoi(code) < 0) {
		return 0;
	} else {
		hashmap_put(errorMessages, code, message);
		return 1;
	}
}
#pragma warning(pop)

char * getErrorMessage(char * code)
{
	struct hashmap_element * e;
	if ((e = hashmap_get(errorMessages, code)) != NULL)
	{
		return (char *)e->data;
	} else {
		return NULL;
	}
}

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

	insertError("1", "Syntax Error. Parenthesis mismatch");
	insertError("2", "Syntax Error. You have two consequtive operators in the expression somewhere");
	
	insertFunction("*", multiply);
	insertFunction("/", divide);
	insertFunction("+", add);
	insertFunction("-", subtract);

	calculatorIsInitialized = 1;
}

void deinitialize_calculator()
{
	hashmap_free(errorMessages);
	func_hashmap_free(functions);
}


int32_t beginInputLoop()
{
	struct expression_pack * expression = (struct expression_pack *)MallocOrDie(sizeof(struct expression_pack));
	expression->error = NULL;
	expression->previousEvaluation = 0;
	expression->expression = (char *)MallocOrDie(MAXIMUM_EXPRESSION_LENGTH * sizeof(char));
	expression->expressionBufferSize = MAXIMUM_EXPRESSION_LENGTH * sizeof(char);

	do {
		if (getline(expression->expression)) {
			while (getchar() != '\n');
			continue;
		}

		if (expression->expression[0] == 'q') break;
		parse(expression);
		if (expression->error) {
			printf("\n%s\n", getErrorMessage(expression->error));
			expression->error = NULL;
		} else {
			printf("\n\n%f\n\n", expression->previousEvaluation);
		}
	} while (expression->expression[0] != 'q');

	free(expression->expression);
	free(expression);

	return 0;
}

#endif