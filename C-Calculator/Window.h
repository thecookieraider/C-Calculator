#ifndef CCALC_WINDOW
#define CCALC_WINDOW

//Originally was going to be unicode, but rewriting the calculator backend was...horrible....
//Fuck that shit
#if ( ( defined(UNICODE) || defined(_UNICODE) ) && defined(_WIN32))// && !defined(USE_UNICODE) No, you cant undo it...
#undef UNICODE
#undef _UNICODE
#endif


#include <tchar.h>
#include <stdlib.h>
#include <Windows.h>
#include "Handlers.h"
#include "resource.h"
HINSTANCE appInst;

static const TCHAR WINDOWCLASS[] = _T("ccalc");
static const TCHAR TITLE[] = _T("C-Calculator");

void paint(HWND handle);
BOOL RegisterApplication(HINSTANCE hinstance);
BOOL InitWindow(HINSTANCE hinstance, int cmdShow);
LRESULT CALLBACK WindowProc(HWND _In_ handle, UINT _In_ msg, WPARAM _In_ wparam, LPARAM _In_ lparam);
int CALLBACK WinMain(_In_ HINSTANCE instance, _In_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdShow);
INT_PTR CALLBACK DialogProc(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
#endif