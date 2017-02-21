#include "C_Calculator_Core.h" 

_CCALC_DT ipow(_CCALC_DT base, _CCALC_DT exp)
{
	if (exp >= 31) {
		__errFlag = FAT_EXPONENT;
		printf("\nFATAL ERROR: Integer Buffer Overflow. Cannot set a number to the power of 31 or greater.");
		__errMsg = "\nFATAL ERROR: Integer Buffer Overflow. Cannot set a number to the power of 31 or greater.";
		return 0;
	}

	bool negExp = false;

	if (exp == 0) return 1;
	if (exp < 0) {
		exp = -exp;
		negExp = true;
	}

	if (exp == .5) return sqrt(base);

	_CCALC_DT result = pow(base, exp);

	if (result == 0 && base != 0) {
		__errFlag = FAT_EXPONENT;
		printf("\nFATAL ERROR: Integer Buffer Overflow. Cannot set a number to the power of 31 or greater.");
		__errMsg = "\nFATAL ERROR: Integer Buffer Overflow. Cannot set a number to the power of 31 or greater.";
		return 0;
	}
	return negExp ? 1 / result : result;
}

char getnextop(char * string, int fromIndex) {
	if (fromIndex < 0)
		return -1;

	int i;
	i = fromIndex;
	do {
		for (; ( !isoperator(string[i]) ) && string[i] != '\0'; i++)
			;

		if (string[i] == '\0')        
			return -1;

		if (isoperator(string[i])) {
			return string[i];
		} 
	} while(1);
}

_CCALC_DT getnextnum(char * string, size_t fromindex) {
	if (fromindex < 0 || fromindex >= strlen(string) + 1)
		return 0;
	
	int i;
	_CCALC_DT val;
	int j;

	float sign = 1;

	char * storage = malloc(sizeof(char) * MAXNUMBERLEN);
	i = fromindex;
	
	
	while (!isdigit(string[i]) && string[i] != '\0' && string[i] != '-' && string[i] != '.')
		i++;
	
	if (string[i] == '\0') {
		__errFlag = GEN_SYNTAX;
		__errMsg = "Syntax Error";
		return -1;
	}

	if (string[i] == '-') {
		sign = -1;
		i++;
	}

	
	for (j = 0; isdigit(string[i]) || string[i] == '.'; i++, j++) 
		storage[j] = string[i];

	storage[j] = '\0';

	val = atof(storage) * sign;

	free(storage);

 	return val;
}

int getnumlength(char * string, size_t fromindex) {
	if (fromindex < 0 || fromindex >= strlen(string) + 1)
		return 0;

	int i = fromindex;

	int len = 0;

	while (!isdigit(string[i]) && string[i] != '\0')
		i++;

	if (string[i] == '\0')
		return 0;

	if (string[i] == '-')
		len++;

	for (; isdigit(string[i]) || string[i] == '.'; i++)
		len++;

	return len;
}


int isoperator(char  op) {
	if (op == 0) return false;
	return strchr(OPERANDS, op) != NULL;
}

_CCALC_DT exec(char * command, _CCALC_DT * ans) {
	command = eatSpaces(command);
	return eval(command, ans);
}

	
int parenthesisAreConsistent(char * expression) {
	int openCnt = 0;
	int closedCnt = 0;

	for (size_t i = 0; i <= strlen(expression) + 1; i++) {
		if (openCnt < closedCnt) return 0;

		if (expression[i] == '\0' && (closedCnt == openCnt))
			return 1;
		else if (expression[i] == '\0' && (closedCnt != openCnt))
			return 0;
		 

		if (expression[i] == '(')
			openCnt++;
		else if (expression[i] == ')')
			closedCnt++;

	}

	return 0;
}

int hasParenthesis(char * expression) {
	for (size_t i = 0; i <= strlen(expression) + 1; i++) {
		if (expression[i] == '\0')
			return 0;
		
		if (expression[i] == '(' || expression[i] == ')')
			return 1;
	}

	return 0;
}

_CCALC_DT executeFunction(char * func, _CCALC_DT arg) {
	if (strcmp(func, "tan") == 0) {
		return tan(arg);
	}
	else if (strcmp(func, "cos") == 0) {
		return cos(arg);
	}
	else if (strcmp(func, "sin") == 0) {
		return sin(arg);
	}
	else if (strcmp(func, "log") == 0) {
		return log(arg);
	}
	else if (strcmp(func, "l10") == 0) {
		return log10(arg);
	}
	else if (strcmp(func, "cosh") == 0) {
		return acos(arg);
	}
	else if (strcmp(func, "sinh") == 0) {
		return asin(arg);
	}
	else if (strcmp(func, "tanh") == 0) {
		return atan(arg);
	}
	else {
		__errMsg = "Invalid function";
		__errFlag = BAD_FUNC;
		return -1;
	}
}
static char * expand(char * expr, _CCALC_DT * ans) {
	//Expand 'ans' variable
	if (strstr(expr, "ans") != NULL && ans != -1) {
		//Get the index
		ptrdiff_t index = strstr(expr, "ans") - expr;
		int m = moveup(*ans);
		char * buffer = malloc(sizeof(char*)*MAXNUMBERLEN);
		sprintf(buffer, "%lf", *ans);

		char * new_expr = (char*)malloc(sizeof(char*)*strlen(expr + 1));
		size_t j, i;
		for (j = 0, i = 0; i <= (strlen(expr) - 3) + m; i++, j++) {
			if (i == index) {
				strcpy(new_expr + j, buffer);
				i += 2;
				j += m - 1;
			}
			else {
				new_expr[j] = expr[i];
			}
		}
		new_expr[j] = '\0';
		free(buffer);
		return expand(new_expr, ans);
	}

	if (strstr(expr, "PI") != NULL) {
		//Get the index
		ptrdiff_t index = strstr(expr, "PI") - expr;
		int m = moveup(M_PI);
		char * buffer = malloc(sizeof(char*)*MAXLINE);
		sprintf(buffer, "%lf", M_PI);

		char * new_expr = (char*)malloc(sizeof(char*)*strlen(expr + 1));
		size_t j, i;
		for (j = 0, i = 0; i <= strlen(expr); i++, j++) {
			if (i == index) {
				strcpy(new_expr + j, buffer);
				i += 2;
				j += m - 1;
			}
			else {
				new_expr[j] = expr[i];
			}
		}
		new_expr[j] = '\0';
		free(buffer);
		return expand(new_expr, ans);
	}

	if (parenthesisAreConsistent) {
		size_t buffLen = strlen(expr);
		char * newExpr = malloc(sizeof(char*)*MAXLINE);
		char * func = malloc(sizeof(char*) * MAX_FUNC_LEN+1);
		char * funcArg = malloc(sizeof(char*) * MAXLINE);
		char * buffer = malloc(sizeof(char*) * MAXLINE);
		size_t i;
		for (i = 0; i < buffLen; i++) {
			if (isalpha(expr[i]) && isalpha(expr[i + 1]) && isalpha(expr[i + 2]) && expr[i + 3] == '(') {
				func[0] = expr[i];
				func[1] = expr[i + 1];
				func[2] = expr[i + 2];
				func[3] = '\0';

				size_t hardBegin = i;
				ptrdiff_t funcBegin = (expr + (i + 3)) - expr;
				ptrdiff_t funcEnd = strchr(expr + funcBegin, ')') - expr;
				int j, z;
				for (j = 0, z = funcBegin+1; z < funcEnd; j++, z++) {
					funcArg[j] = expr[z];
				}

				funcArg[j] = 0;
				
				_CCALC_DT funcArgExpanded = eval(funcArg, ans);
				_CCALC_DT funcResult = executeFunction(func, funcArgExpanded);
				sprintf(buffer, "%lf", funcResult);
				int m = moveup(funcResult);
				free(funcArg);
				for (j = 0, z = 0; z < buffLen + 1; z++, j++) {
					if (z == hardBegin) {
						strcpy(newExpr + z, buffer);
						z += funcEnd - funcEnd;
						j += m;
					}
					else {
						newExpr[j] = expr[z];
					}
				}
				newExpr[j] = 0;
				return expand(newExpr, ans);
			}
			

		}
	}
	else {
		__errFlag = BAD_PARENTHESIS;
		printf("Syntax Error: Parenthesis Mismatch. Try Again\n");
		__errMsg = "Syntax Error: Parenthesis Mismatch. Try Again\n";
		return '\0';
	}

	if (hasParenthesis(expr)) {
		if (parenthesisAreConsistent(expr)) {
			char * b = strchr(expr, '(');
			ptrdiff_t beginIndex = b - expr;

			char * e = strchr(expr + beginIndex, ')');
			ptrdiff_t endIndex = e - expr;

			char * placeholder = (char*)malloc(sizeof(char*)*strlen(expr + 1));
			size_t i;
			size_t j;

			for (i = beginIndex + 1, j = 0; i < endIndex; i++, j++) {
				placeholder[j] = expr[i];
			}

			placeholder[j] = '\0';

			_CCALC_DT computedVal = eval(placeholder, ans);
			if (computedVal == -1) return 0;
			int m = moveup(computedVal);
			char * buffer = malloc(sizeof(char*) * MAXNUMBERLEN);
			sprintf(buffer, "%lf", computedVal);
			free(placeholder);
			char * new_expr = (char*)malloc(sizeof(char*)*strlen(expr + 1));
			for (j = 0, i = 0; i <= strlen(expr) + 1; i++, j++) {
				if (i == beginIndex) {
					strcpy(new_expr + j, buffer);
					i += (endIndex - beginIndex);
					j += m - 1;
				}
				else {
					new_expr[j] = expr[i];
				}
			}
			new_expr[j] = '\0';
			free(buffer);
			return expand(new_expr, ans);
		}
		else {
			__errFlag = BAD_PARENTHESIS;
			printf("Syntax Error: Parenthesis Mismatch. Try Again\n");
			__errMsg = "Syntax Error: Parenthesis Mismatch. Try Again\n";
			return '\0';
		}
	}

	if (strstr(expr, "!")) {
		int i, j;
		char * index = strchr(expr, '!');
		ptrdiff_t beginIndex = (index - expr);
		int numLen = getnumlength(expr, beginIndex);
		char * afterFactorial = malloc(sizeof(char*)*strlen(expr) + 1);

		for (i = beginIndex+1, j = 0; i <= numLen + beginIndex; i++, j++) {
			afterFactorial[j] = expr[i];
		}

		afterFactorial[j] = '\0';
		if (atof(afterFactorial) > MAX_FACTORIAL) return "0";
		_CCALC_DT afterFactorialExp = factorial(atof(afterFactorial));
		char * buffer = malloc(sizeof(char*) * MAXNUMBERLEN);
		sprintf(buffer, "%lf", afterFactorialExp);
		free(afterFactorial);
		char * new_expr = malloc(sizeof(char*) * strlen(expr) + strlen(buffer) + 1);
		for (j = 0, i = 0; i <= strlen(expr) + 1; i++, j++) {
			if (i == beginIndex) {
				strcpy(new_expr + j, buffer);
				i += numLen;
				j += strlen(buffer);
			}
			else {
				new_expr[j] = expr[i];
			}
		}
		new_expr[j] = '\0';
		free(buffer);
		return expand(new_expr, ans);
	}

	return expr;


}

_CCALC_DT eval(char * expr, _CCALC_DT * ans) {
	if (__errFlag) return -1;
	expr = expand(expr, ans);
	if (__errFlag) return -1;
	if (getnextop(expr, 1) == -1) return atof(expr);
	
	
	int position = 0;
	
	int position_in_array = 0;
	
	
	
	_CCALC_DT first, second;
	
	
	int m;
	char operand;
	
	int exprCount = 0;
	
	struct expression expr_vec[MAXLINE / 3];
	
	first = getnextnum(expr, position);
	
	
	m = getnumlength(expr, position);
	
	if (first < 0) m += 1;
	
	position += m;
	
	operand = getnextop(expr, position++);
	second = getnextnum(expr, position);

	if (__errFlag == GEN_SYNTAX) return -1;
	m = getnumlength(expr, position);
	if (second < 0) m += 1;
	position += m;
	
	exprCount = 1;
	
	struct expression first_expr = { operand, first, second, 0, getOperandPriority(operand) };
	
	expr_vec[position_in_array] = first_expr;
	position_in_array++;
	
	while(1) {
		
		if (expr[position] == '\0') {
			break;
		}
		
		first = expr_vec[position_in_array - 1].righthand;
		
		operand = getnextop(expr, position++);
		second = getnextnum(expr, position);
		
		m = getnumlength(expr, position);
		if (second < 0) m += 1;
		position += m;
		
		if (__errFlag == GEN_SYNTAX) return -1;

		if (first == -1 || operand == -1 || second == -1) {
			break;
		}
		
		struct expression exp = { operand, first, second, 0, getOperandPriority(operand) };
		expr_vec[position_in_array] = exp;
		position_in_array++;
		exprCount++;
	}
	
	
	int maxprioIndex;
	int priority = 0;
	for (int i = 0; i < exprCount; i++) {
		if (expr_vec[i].priority > priority) {
			maxprioIndex = i;
			priority = expr_vec[i].priority;
		}
	}
	
	expr_vec[maxprioIndex].flagged = 1;
	_CCALC_DT result;
	
	switch (expr_vec[maxprioIndex].operand) {
	case '*':
		result = expr_vec[maxprioIndex].lefthand * expr_vec[maxprioIndex].righthand;
		break;
	case '/':
		result = expr_vec[maxprioIndex].lefthand / expr_vec[maxprioIndex].righthand;
		break;
	case '-':
		result = expr_vec[maxprioIndex].lefthand - expr_vec[maxprioIndex].righthand;
		break;
	case '+':
		result = expr_vec[maxprioIndex].lefthand + expr_vec[maxprioIndex].righthand;
		break;
	case '^':
		result = ipow(expr_vec[maxprioIndex].lefthand, expr_vec[maxprioIndex].righthand);

	}

	if (result == 0 && expr_vec[maxprioIndex].operand == '^') {
		__errFlag = FAT_EXPONENT;
		printf("\nFATAL ERROR: Integer Buffer Overflow. Max value is 2^31. Result was larger than or equal to this\n\n");
		__errMsg = "\nFATAL ERROR : Integer Buffer Overflow.Max value is 2 ^ 31. Result was larger than or equal to this\n\n";
		goto destruct;
	}

	
	int resultIsIn = 0;
	char * newexpr = malloc(sizeof(char *) * strlen(expr));
	
	for (int i = 0; i < exprCount; i++) {
		
		if (expr_vec[i].flagged == 1 && resultIsIn) continue;
		
		if (exprCount == 1) {
			sprintf(newexpr, "%lf", result);
			break;
		}
		
		
		char * to_add = malloc(sizeof(char *) * strlen(expr));
		
		
		
		if (expr_vec[i].flagged == 1 && i == 0) {
			sprintf(to_add, "%lf%c%lf", result, expr_vec[i+1].operand, expr_vec[i+1].righthand);
			
			
			strcpy(newexpr, to_add);
			
			i++;
			continue;
		}
		
		
		
		else if (expr_vec[i].flagged == 1 && i != 0) {
			sprintf(to_add, "%c%lf", expr_vec[i + 1].operand, result);
			
			strncat(newexpr, to_add, strlen(to_add));
			i++;
			continue;
		}
		
		
		
		else if (expr_vec[i + 1].flagged == 1 && i == 0) {
			sprintf(to_add, "%lf%c%lf", expr_vec[i].lefthand, expr_vec[i].operand, result);
			
			if(i == 0) strcpy(newexpr, to_add);
			else strncat(newexpr, to_add, strlen(to_add));
			resultIsIn = 1;
			continue;
		}
		
		
		
		else if (expr_vec[i + 1].flagged == 1 && i != 0) {
			sprintf(to_add, "%c%lf", expr_vec[i].operand, result);
			
			if (i == 0) strcpy(newexpr, to_add);
			else strncat(newexpr, to_add, strlen(to_add));
			resultIsIn = 1;
			continue;
		}
		
		
		if (i == 0) sprintf(to_add, "%lf%c%lf", expr_vec[i].lefthand, expr_vec[i].operand, expr_vec[i].righthand);
		else sprintf(to_add, "%c%lf", expr_vec[i].operand, expr_vec[i].righthand);
		if (i == 0) strcpy(newexpr, to_add);
		else strncat(newexpr, to_add, strlen(to_add));

		free(to_add);
	}
	//free(expr);
	
	return eval(newexpr, ans);

destruct:
	free(expr);
	return -1;
}

_CCALC_DT factorial(_CCALC_DT num)
{
	if (num < -1) {
		__errFlag = NEG_FACTORIAL;
		__errMsg = "Factorial cannot be negative.";
	}
	_CCALC_DT result;
	if (num > MAX_FACTORIAL) {
		__errFlag = FAT_FACTORIAL;
		printf("Factorial is too fat man. Max for the factorial is " + MAX_FACTORIAL);
		__errMsg = "Factorial is too fat man. Max for the factorial is " + MAX_FACTORIAL;
		return -1;
	}
	if (num == 0 || num == 1)
		return 1;

	result = factorial(num - 1) * num;
	return result;
}