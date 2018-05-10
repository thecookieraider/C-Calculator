#ifndef LOGGER_H
#define LOGGER_H

#define FILE_OK 1
#define FILE_NOTOK -1
#define BUFFER_DOESNT_EXIST -2

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "hashmap.h"

struct hashmap * fileBuffers = NULL;

void logger_setupBuffer(char * fileName)
{
	if (!fileBuffers) fileBuffers = new_hashmap();
//fopen is safe enough to use, so disable the warning that comes with it
#pragma warning(push)
#pragma warning(disable : 4996)
	FILE * f = fopen(fileName, "w+");
#pragma warning(pop)
	if (!f) {
		perror("Couldn't setup a logging buffer. Fatal.");
		exit(1);
	}

	hashmap_put(fileBuffers, fileName, (void *)f);
}


int32_t logger_setupBuffers(int count, char * fileNames, ...)
{
	logger_setupBuffer(fileNames);
	va_list args;
	va_start(args, fileNames);

	for (int32_t i = 0; i < count - 1; i++) {
		char * arg = va_arg(args, char*);
		logger_setupBuffer(arg);
	}

	va_end(args);
	return 1;
}

#pragma warning(push)
#pragma warning(disable : 4996)
int32_t logger_writeToBuffer(char * fileName, char * toWrite)
{
	//Get the current date time
	time_t rawTime = time(NULL);

	struct hashmap_element * e;
	if ((e = hashmap_get(fileBuffers, fileName)) != NULL) {
		FILE * f = (FILE *)e->data;
		fputs(asctime(localtime(&rawTime)), f);
		fputc(' ', f);
		fputs(toWrite, f);
		return ferror(f) ? FILE_NOTOK : FILE_OK;
	} else {
		return BUFFER_DOESNT_EXIST;
	}
}
#pragma warning(pop)


int32_t logger_closeBuffer(char * fileName, int32_t flushBuffer)
{
	struct hashmap_element * e;
	if ((e = hashmap_get(fileBuffers, fileName)) != NULL) {
		FILE * f = (FILE *)e->data;
		if (flushBuffer) fflush(f);
		fclose(f);
		hashmap_remove(fileBuffers, fileName);
		return FILE_OK;
	} else {
		return BUFFER_DOESNT_EXIST;
	}
}

int32_t logger_retrieveBuffer(char * fileName, char * bufferToWriteTo)
{
	struct hashmap_element * e;
	if ((e = hashmap_get(fileBuffers, fileName)) != NULL) {
		FILE * f = (FILE *)e->data;
		long fileSize;

		fseek(f, 0L, SEEK_END);
		fileSize = ftell(f);
		rewind(f);

		bufferToWriteTo = (char *)CallocOrDie(1, fileSize + 1);
		fread(bufferToWriteTo, sizeof(char), fileSize, f);

		if (ferror(f)) return FILE_NOTOK;
		else return FILE_OK;
	} else {
		return BUFFER_DOESNT_EXIST;
	}
}

FILE * logger_retrieveBuffer(char * fileName)
{
	struct hashmap_element * e;
	if ((e = hashmap_get(fileBuffers, fileName)) != NULL) {
		return (FILE *)e->data;
	} else {
		return BUFFER_DOESNT_EXIST;
	}
}

void logger_free()
{
	for (int32_t i = 0; i < fileBuffers->size; i++) {
		if (fileBuffers->elements[i].key != NULL) {
			fclose((FILE *)fileBuffers->elements[i].data);
		}
	}
	hashmap_free(fileBuffers);
}

#endif