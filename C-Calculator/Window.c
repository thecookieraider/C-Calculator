#include "Window.h"

INT_PTR CALLBACK DialogProc(HWND hdlg, UINT umsg, WPARAM wparam, LPARAM lparam) {
	switch (umsg)
	{
	case WM_CTLCOLORDLG:
		return (INT_PTR)GetSysColorBrush(COLOR_WINDOW);

	case WM_COMMAND:
		if (HIWORD(wparam) == BN_CLICKED) {
			keypadHandler(hdlg, umsg, wparam, lparam);
			return TRUE;
		}

		switch (LOWORD(wparam))
		{
		case IDC_EDIT1:
			textFieldHandler(hdlg, umsg, wparam, lparam);
			return TRUE;
		}


		break;

	case WM_CLOSE:
		free(ans);
		DestroyWindow(hdlg);
		return TRUE;

	case WM_DESTROY:
		free(ans);
		PostQuitMessage(0);
		return TRUE;
	}
	return FALSE;

}

int CALLBACK WinMain(_In_ HINSTANCE instance, _In_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdShow)
{
	HWND hdlg;
	ans = malloc(sizeof(_CCALC_DT));
	ans = 0;
	hdlg = CreateDialogParam(0, MAKEINTRESOURCE(CCALC_GUI), 0, DialogProc, 0);
	ShowWindow(hdlg, cmdShow);
	
	/*if (!RegisterApplication(instance)) {
		MessageBox(NULL, _T("Could not register application"), _T("ERROR"), MB_OK);
		return 0;
	}
	if (!InitWindow(instance, cmdShow)) {
		MessageBox(NULL, _T("Could not initialize window"), _T("ERROR"), MB_OK);
		return 0;
	}*/
	
	MSG msg;
	BOOL mresult;
	while ((mresult = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (mresult == -1) {
			PostQuitMessage(0);
		}

		if (!IsDialogMessage(hdlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
	
}


BOOL RegisterApplication(HINSTANCE hinstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_VREDRAW | CS_HREDRAW;

	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOWCLASS;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitWindow(HINSTANCE hinstance, int cmdShow) {
	HWND hwnd;

	appInst = hinstance;

	hwnd = CreateWindow(
		WINDOWCLASS,
		TITLE,
		WS_OVERLAPPEDWINDOW | WS_BORDER, //style 
		CW_USEDEFAULT,
		CW_USEDEFAULT, //width
		CW_USEDEFAULT, //height
		CW_USEDEFAULT, 
		NULL, NULL, 
		hinstance, 
		NULL);

	if (!hwnd) return FALSE;

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	return TRUE;
}

LRESULT CALLBACK WindowProc(HWND _In_ handle, UINT _In_ msg, WPARAM _In_ wparam, LPARAM _In_ lparam) {
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lparam);
		int y = HIWORD(lparam);
		break;
	}
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE) {
			PostQuitMessage(0);
			DestroyWindow(handle);
			return 0;
		}
		break;
	case WM_PAINT:
		BeginPaint(handle, &ps);
		paint(handle);
		EndPaint(handle, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(handle, msg, wparam, lparam);
		break;
	}

	return 0;
	
		
}

void paint(HWND handle) {
	HDC hdc = GetDC(handle);
	
	ReleaseDC(handle, hdc);
	
	
}