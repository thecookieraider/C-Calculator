#ifndef CCALC_HANDLERS
#define CCALC_HANDLERS
#include "Window.h"
#include "C_Calculator_Core.h"

_CCALC_DT * ans;


void keypadHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
LPTSTR getEditFieldText(HWND hdlg, INT elementCode);
void operandHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
void textFieldHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
void keyboardHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
bool funcHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam);
size_t getEditFieldLen(HWND hdlg, INT elementCode);
#endif