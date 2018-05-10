#include "hashmap.h"
#include "logger.h"
#include <stdio.h>
#include <Windows.h>

int32_t main()
{
	logger_setupBuffer("test.txt");
	logger_writeToBuffer("test.txt", "this is a test\n");
	logger_writeToBuffer("test.txt", "dont be alarmed");
	logger_closeBuffer("test.txt", 0);
}