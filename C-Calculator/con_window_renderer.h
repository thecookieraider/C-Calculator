#ifndef CON_WINDOW_RENDERER
#define CON_WINDOW_RENDERER

#include "util.h"
#include "con_window_buffer.h"
#include <stdint.h>

struct ConWindowRenderer
{
	struct ConWindowBuffer * buf;
	short yOff, xOff;
	short renderBoundsW, renderBoundsH;
};

void init_renderer(struct ConWindowRenderer * r, CONSOLE_SCREEN_BUFFER_INFO cbi)
{
	init_gbuffer(r->buf, cbi);
	r->xOff = r->yOff = 0;
}

void renderer_draw(ConWindowRenderer * r, int32_t x, int32_t y, WORD fg, WORD bg, TCHAR newChar)
{
	gbuffer_setChar(r->buf, x, y, newChar);
	gbuffer_setColor(r->buf, x, y, fg, bg);
}

void renderer_render(ConWindowRenderer * r, HANDLE oh, CONSOLE_SCREEN_BUFFER_INFO cbi)
{
	SMALL_RECT sr = { 0, 0, r->renderBoundsW, r->renderBoundsH };
	WriteConsoleOutput(oh, r->buf->chars, cbi.dwSize, { r->xOff, r->yOff }, &sr);
}

void renderer_drawString(ConWindowRenderer * r, const TCHAR * str, size_t len, int32_t x, int32_t y, WORD fg, WORD bg)
{
	for (unsigned int32_t i = x, j = 0; j < len && i < x + len; j++, i++) {
		gbuffer_setChar(r->buf, i, y, str[j]);
		gbuffer_setColor(r->buf, i, y, fg, bg);
	}
}

void renderer_drawVerticalString(ConWindowRenderer * r, const TCHAR * str, size_t len, int32_t x, int32_t y, WORD fg, WORD bg)
{
	for (unsigned int32_t j = 0; j < len; j++) {
		gbuffer_setChar(r->buf, x, y, str[j]);
		gbuffer_setColor(r->buf, x, y++, fg, bg);
	}
}

int32_t renderer_centerObjectXCoord(ConWindowRenderer * r, int32_t objectW)
{
	return((r->renderBoundsW / 2 + r->xOff) - objectW / 2) - 1;
}

int32_t renderer_centerObjectYCoord(ConWindowRenderer * r, int32_t objectH)
{
	return((r->renderBoundsH / 2 + r->yOff) - objectH / 2) - 1;
}
#endif