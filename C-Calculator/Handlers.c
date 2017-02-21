#include "Handlers.h"

bool clearOnType = false;

size_t getEditFieldLen(HWND hdlg, int elementCode) {
	int bufferLen = SendDlgItemMessage(hdlg, elementCode, WM_GETTEXTLENGTH, 0, 0) + 1;
}

LPTSTR getEditFieldText(HWND hdlg, int elementCode) {
	LPTSTR text = malloc(sizeof(LPTSTR) * (int)SendDlgItemMessage(hdlg, elementCode, WM_GETTEXTLENGTH, 0, 0) + 1);
	int bufferLen = getEditFieldLen(hdlg, IDC_EDIT1);
	GetDlgItemText(hdlg, elementCode, text, bufferLen);

	return text;
}

void textFieldHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	int	notificationCode = HIWORD(wparam);
	if (notificationCode == EN_CHANGE) {
		if (clearOnType) {
			LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
			LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
			int bufferLen = getEditFieldLen(hdlg, IDC_EDIT1);
			if (editControlBuffer[0] == '\0') {
				MessageBeep((UINT)-1); 
				return;
			}
			if (editControlBuffer[bufferLen - 2] == '!') {
				sprintf(newEditControl, "%c%s", '!', "ans");
				SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
				clearOnType = false;
				free(editControlBuffer);
				return;
			}
			if (isoperator(editControlBuffer[bufferLen - 2])) {
				sprintf(newEditControl, "%s%c", "ans", editControlBuffer[bufferLen - 2]);
				SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
				clearOnType = false;
				free(editControlBuffer);
				return;
			}

			if (strstr(editControlBuffer, "ans")){
				int i;
				for (i = 0; i < bufferLen-1; i++) {
					newEditControl[i] = editControlBuffer[i];
				}
				newEditControl[i] = '\0';
				
				SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
				clearOnType = false;
				free(editControlBuffer);
				return;
			}
			sprintf(newEditControl, "%c", editControlBuffer[bufferLen - 2]);
			SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
			clearOnType = false;
			free(editControlBuffer);
			return;
		}
		if (getEditFieldLen(hdlg, IDC_EDIT1) > MAXLINE) {
			LPTSTR text = malloc(sizeof(LPTSTR) * (int)SendDlgItemMessage(hdlg, IDC_EDIT1, WM_GETTEXTLENGTH, 0, 0) + 1);
			LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(text) + 1);
			int bufferLen = getEditFieldLen(hdlg, IDC_EDIT1);

			GetDlgItemText(hdlg, IDC_EDIT1, text, bufferLen);
			lstrcpyn(newEditControl, text, MAXLINE);
			SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
			free(text);
			MessageBox(hdlg, _T("Expression cannot exceed 100 characters"), _T("Buffer Overflow"), MB_OK);
		}
	}
}

bool funcHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	int buttonid = LOWORD(wparam);
	switch (buttonid) {
	case BUTTONBACK:{
			LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
			LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
			int bufferLen = getEditFieldLen(hdlg, IDC_EDIT1);
			if (editControlBuffer[0] == '\0') {
				MessageBeep((UINT)-1); 
				return true;
			}
			int i;
			if (isalpha(editControlBuffer[bufferLen - 2])) bufferLen -= 4;
			else bufferLen -= 2;
			for (i = 0; i < bufferLen; i++) {
				newEditControl[i] = editControlBuffer[i];
			}

			newEditControl[i] = '\0';
			SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
			clearOnType = false;
			free(editControlBuffer);
			return true;
		}
	case BUTTONDEC: {
		if (strchr(getEditFieldText(hdlg, IDC_EDIT1), '.')) return true;
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("."));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		return true;
		}
	case BUTTONCLEAR2: {
		clearOnType = false;
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, (LPTSTR)" ");
		return true;
		}

	case BUTTONSIGN: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		int i;
		if (strstr(getEditFieldText(hdlg, IDC_EDIT1), _T("-"))) {
			for (i = 1; i < getEditFieldLen(hdlg, IDC_EDIT1); i++) {
				newEditControl[i-1] = editControlBuffer[i];
			}
			newEditControl[i - 1] = '\0';
			SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
			return true;
		}
		else {
			newEditControl[0] = '-';
			for (i = 0; i < getEditFieldLen(hdlg, IDC_EDIT1); i++) {
				newEditControl[i+1] = editControlBuffer[i];
			}
			newEditControl[i] = '\0';
			SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
			return true;
		}
		
		
	}
	case BUTTONOPENP: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("("));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}
	case BUTTONCLOSEP: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T(")"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}

	case BUTTONTAN: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("tan("));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}

	case BUTTONCOS: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("cos("));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}
	
	 case BUTTONSIN: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("sin("));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}
	
	case BUTTONPI: {
		LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
		LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
		lstrcpy(newEditControl, editControlBuffer);
		lstrcat(newEditControl, _T("PI"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	}
	}

	return false;
}

void operandHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	if (funcHandler(hdlg, umsg, wparam, lparam)) return;
	LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
	LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);
	int buttonid = LOWORD(wparam);
	lstrcpy(newEditControl, editControlBuffer);

	int bufferLen = getEditFieldLen(hdlg, IDC_EDIT1);
	if (isoperator(editControlBuffer[bufferLen - 2]) && buttonid != BUTTONENTER && buttonid != BUTTONANS) {
		Beep(750, 250);
		MessageBox(hdlg, _T("Cannot have two consecutive operators"), _T("Syntax Error"), MB_OK);
		return;
	}
	HWND edithandle = GetDlgItem(hdlg, IDC_EDIT1);

	switch (buttonid) {
	case BUTTONMULT:
		lstrcat(newEditControl, _T("*"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONDIV:
		lstrcat(newEditControl, _T("/"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONADD:
		lstrcat(newEditControl, _T("+"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONSUB:
		lstrcat(newEditControl, _T("-"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONANS:
		lstrcat(newEditControl, _T("ans"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONEXP:
		lstrcat(newEditControl, _T("^"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONSQRT:
		lstrcat(newEditControl, _T("^(0.5)"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONFACT:
		lstrcat(newEditControl, _T("!"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTONENTER: {
		_CCALC_DT evaluation = exec(getEditFieldText(hdlg, IDC_EDIT1), ans);
		*ans = evaluation;

		if (__errFlag) {
			MessageBox(hdlg, _T(__errMsg), "Error", MB_OK);
			__errFlag = 0;
			break;
		}

		LPTSTR * outputBuffer = malloc(sizeof(char*) * MAXNUMBERLEN);
		sprintf(outputBuffer, "%lf", evaluation);
		lstrcpy(newEditControl,outputBuffer);
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		clearOnType = true;
		break;
	}
	}
}

void keypadHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	LPTSTR editControlBuffer = getEditFieldText(hdlg, IDC_EDIT1);
	LPTSTR newEditControl = malloc(sizeof(LPTSTR) * wcslen(editControlBuffer) + 2);

	lstrcpy(newEditControl, editControlBuffer);

	int buttonid = LOWORD(wparam);
	HWND edithandle = GetDlgItem(hdlg, IDC_EDIT1);
	switch (buttonid) {

	case BUTTON1:
		lstrcat(newEditControl, _T("1"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON2:
		lstrcat(newEditControl, _T("2"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON3:
		lstrcat(newEditControl, _T("3"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON4:
		lstrcat(newEditControl, _T("4"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON5:
		lstrcat(newEditControl, _T("5"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON6:
		lstrcat(newEditControl, _T("6"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON7:
		lstrcat(newEditControl, _T("7"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON8:
		lstrcat(newEditControl, _T("8"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON9:
		lstrcat(newEditControl, _T("9"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;
	case BUTTON0:
		lstrcat(newEditControl, _T("0"));
		SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, newEditControl);
		break;

	default:
		operandHandler(hdlg, umsg, wparam, lparam);
	}

}

void keyboardHandler(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	if (GetAsyncKeyState(VK_ESCAPE)) {
		free(ans);
		DestroyWindow(hdlg);
		PostQuitMessage(0);
	}

}
