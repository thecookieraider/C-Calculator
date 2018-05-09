#ifndef CON_WINDOW_H
#define CON_WINDOW_H

#include <Windows.h>
#include <tchar.h>
#include "con_window_renderer.h"

struct ConWindow
{
	HANDLE oh, ih;
	CONSOLE_SCREEN_BUFFER_INFO cbi;
	HWND handle;
	BOOL(*handlerRoutine)(DWORD);
	ConWindowRenderer * renderer;

	COORD cursorPos;
	CONSOLE_FONT_INFOEX font;

	int windowW, windowH;
	short bufW, bufH, visBufW, visBufH;
	int cursorVisible, isFullScreen;
};

void window_init(struct ConWindow * window)
{
	window->oh = GetStdHandle(STD_OUTPUT_HANDLE);
	window->ih = GetStdHandle(STD_INPUT_HANDLE);
	window->handle = GetConsoleWindow();

	RECT windowRect;
	GetWindowRect(GetConsoleWindow(), &windowRect);
	GetConsoleScreenBufferInfo(window->oh, &window->cbi);
	window->cursorPos = window->cbi.dwCursorPosition;

	window->windowW = windowRect.right - windowRect.left + 1;
	window->windowW = windowRect.bottom - windowRect.top + 1;
	window->bufW = window->cbi.dwSize.X;
	window->bufH = window->cbi.dwSize.Y;

	window->visBufW = window->cbi.srWindow.Right - window->cbi.srWindow.Left;
	window->visBufH = window->cbi.srWindow.Bottom - window->cbi.srWindow.Top;

	window->renderer->renderBoundsH = window->visBufH;
	window->renderer->renderBoundsW = window->visBufW;
	window->cursorVisible = true;
	window->isFullScreen = false;
}

void window_resizeBuffer(struct ConWindow * window, short w, short h)
{
	GetConsoleScreenBufferInfo(window->oh, &window->cbi);
		
	SMALL_RECT& winInfo = window->cbi.srWindow;
	COORD windowSize = { winInfo.Right - winInfo.Left + 1, winInfo.Bottom - winInfo.Top + 1 };

	if (windowSize.X > w || windowSize.Y > h) {
		// window size needs to be adjusted before the buffer size can be reduced.
		SMALL_RECT info =
		{
			0,
			0,
			w < windowSize.X ? w - 1 : windowSize.X - 1,
			h < windowSize.Y ? h - 1 : windowSize.Y - 1
		};

		SetConsoleWindowInfo(window->oh, TRUE, &info);
	}

	COORD size = { w, h };
	SetConsoleScreenBufferSize(window->oh, size);

	GetConsoleScreenBufferInfo(window->oh, &window->cbi);
	window->visBufH = window->cbi.srWindow.Bottom - window->cbi.srWindow.Top;
	window->visBufW = window->cbi.srWindow.Right - window->cbi.srWindow.Left;
	window->renderer->renderBoundsH = window->visBufH;
	window->renderer->renderBoundsW = window->visBufW;

	window->bufW = w;
	window->bufH = h;
	init_renderer(window->renderer, window->cbi);
}

void window_resizeWindow(struct ConWindow * window, short w, short h)
{
	RECT windowRect;
	GetWindowRect(GetConsoleWindow(), &windowRect);

	window->windowW = w;
	window->windowH = h;

	MoveWindow(GetConsoleWindow(), windowRect.left, windowRect.top, w, h, TRUE);

	GetConsoleScreenBufferInfo(window->oh, &window->cbi);
	window->visBufH = window->cbi.srWindow.Bottom - window->cbi.srWindow.Top;
	window->visBufW = window->cbi.srWindow.Right - window->cbi.srWindow.Left;
	window->renderer->renderBoundsH = window->visBufH;
	window->renderer->renderBoundsW = window->visBufW;
}

void window_moveCursor(struct ConWindow * window, short x, short y)
{
	GetConsoleScreenBufferInfo(window->oh, &window->cbi);
	window->cursorPos = { x, y };
	SetConsoleCursorPosition(window->oh, { x, y });
}

void window_render(struct ConWindow * window)
{
	window->renderer->renderBoundsH = window->visBufH;
	window->renderer->renderBoundsW = window->visBufW;
	renderer_render(window->renderer, window->oh, window->cbi);
	window_setCursorVisible(window, window->cursorVisible);
}

void window_setCursorVisible(struct ConWindow * window, int show)
{
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(window->oh, &cci);

	cci.bVisible = show;
	window->cursorVisible = show;
	SetConsoleCursorInfo(window->oh, &cci);
}

void window_setFontSettings(struct ConWindow * window, int w, int h, const TCHAR * fontFaceName)
{
	if (w < 5 || w > 72) w = 28;
	if (h < 5 || h > 72) h = 28;

	window->font = { 0 };
	window->font.cbSize = sizeof(window->font);

	window->font.dwFontSize.Y = w;
	window->font.dwFontSize.X = h;

	window->font.FontWeight = FW_NORMAL;

	_tcscpy_s(window->font.FaceName, fontFaceName);
	SetCurrentConsoleFontEx(window->oh, false, &window->font);

	//Need to undo the console's auto resizement of the buffer and the window when a font is set
	window_resizeWindow(window, window->windowW, window->windowH);
	window_resizeBuffer(window, window->cbi.dwSize.X, window->cbi.dwSize.Y);
}

void window_setTextWeight(struct ConWindow * window, int modifier)
{
	window->font.FontWeight = modifier;
	SetCurrentConsoleFontEx(window->oh, false, &window->font);
}

void window_setFullscreen(struct ConWindow * window, bool set)
{
	if (set) {
		CONSOLE_SCREEN_BUFFER_INFO cbi;
		if (SetConsoleDisplayMode(window->oh, CONSOLE_FULLSCREEN_MODE, &cbi.dwSize)) {
			window->isFullScreen = 1;
			GetConsoleScreenBufferInfo(window->oh, &cbi);
			window->visBufH = cbi.srWindow.Bottom - cbi.srWindow.Top;
			window->visBufW = cbi.srWindow.Right - cbi.srWindow.Left;
			window->renderer->renderBoundsH = window->visBufH;
			window->renderer->renderBoundsW = window->visBufW;
		} 
	} else {
		CONSOLE_SCREEN_BUFFER_INFO cbi;
		if (SetConsoleDisplayMode(window->oh, CONSOLE_WINDOWED_MODE, &cbi.dwSize)) {
			window->isFullScreen = 0;
			GetConsoleScreenBufferInfo(window->oh, &cbi);
			window->visBufH = cbi.srWindow.Bottom - cbi.srWindow.Top;
			window->visBufW = cbi.srWindow.Right - cbi.srWindow.Left;
			window->renderer->renderBoundsH = window->visBufH;
			window->renderer->renderBoundsW = window->visBufW;
		}
	}
}

void window_setConsoleModes(struct ConWindow * window, int * toEnable, int * toDisable, int n, int n2)
{
	DWORD mode;
	GetConsoleMode(window->ih, &mode);

	for (int i = 0; i < n; i++) {
		mode |= toEnable[i];
	}

	for (int i = 0; i < n2; i++) {
		mode &= ~toDisable[i];
	}

	SetConsoleMode(window->ih, mode);
}
#endif