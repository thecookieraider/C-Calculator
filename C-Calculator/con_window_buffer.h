#ifndef CON_WINDOW_BUFFER_H
#define CON_WINDOW_BUFFER_H

#pragma once

#include <Windows.h>
#include <math.h>
#include <stdint.h>
#include "util.h"

enum ConsoleColor
{
	BLACK = 0,
	DARKBLUE = FOREGROUND_BLUE,
	DARKGREEN = FOREGROUND_GREEN,
	DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	DARKRED = FOREGROUND_RED,
	DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	DARKGRAY = FOREGROUND_INTENSITY,
	GRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
};

#define _CTA(bg, fg) (16 * bg) + fg

struct ConWindowBuffer
{
	CHAR_INFO * chars;
	short c, r;
};


void gbuffer_setColor(struct ConWindowBuffer * buf, int32_t x, int32_t y, WORD fg, WORD bg)
{
	buf->chars[(y * buf->c) + x].Attributes = _CTA(bg, fg);
}

void gbuffer_setChar(struct ConWindowBuffer * buf, int32_t x, int32_t y, TCHAR newChar)
{
#if defined(_UNICODE) || defined(UNICODE)
	buf->chars[(y * buf->c) + x].Char.UnicodeChar = newChar;
#else
	buf->chars[(y*buf->c) + x].Char.AsciiChar = newChar;
#endif
}

void init_gbuffer(struct ConWindowBuffer * buf, CONSOLE_SCREEN_BUFFER_INFO cbi)
{
	buf->c = cbi.dwSize.X;
	buf->r = cbi.dwSize.Y;

	if (buf->chars) free(buf->chars);

	buf->chars = (CHAR_INFO *)MallocOrDie(buf->c * buf->r * sizeof(CHAR_INFO));
}

void gbuffer_resize(struct ConWindowBuffer * buf)
{
	CHAR_INFO * newChars = (CHAR_INFO *)MallocOrDie(buf->c * buf->r * 2 * sizeof(CHAR_INFO));
	memcpy(newChars, buf->chars, buf->r * buf->c * sizeof(CHAR_INFO));
	free(buf->chars);
	buf->chars = newChars;
	buf->c *= 2;
	buf->r *= 2;
}

void gbuffer_clear(struct ConWindowBuffer * buf)
{
	for (int32_t i = 0; i < buf->r * buf->c; i++) {
		buf->chars[i].Attributes = _CTA(WHITE, WHITE);
#if defined(_UNICODE) || defined(UNICODE)
		buf->chars[i].Char.UnicodeChar = ' ';
#else
		buf->chars[i].Char.AsciiChar = ' ';
#endif
	}
}

void gbuffer_clear(struct ConWindowBuffer * buf, WORD fg, WORD bg)
{
	for (int32_t i = 0; i < buf->r * buf->c; i++) {
		buf->chars[i].Attributes = _CTA(bg, fg);
#if defined(_UNICODE) || defined(UNICODE)
		buf->chars[i].Char.UnicodeChar = ' ';
#else
		buf->chars[i].Char.AsciiChar = ' ';
#endif
	}
}

void gbuffer_free(struct ConWindowBuffer * buf)
{
	free(buf->chars);
	buf->chars = NULL;
	buf->c = buf->r = 0;
}

#endif