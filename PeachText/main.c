#include "PeachTea/PeachTea.h"

#pragma comment(lib, "PeachTea/PeachTea.lib")
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")

#define WIN32_LEAN_AND_MEAN
#include <dwmapi.h>
#include <Windows.h>
#include <Uxtheme.h>
#include <vssym32.h>

#include "renderer.h"
#include "text_editor.h"

#define RECTHEIGHT(rect) rect.bottom - rect.top
#define RECTWIDTH(rect) rect.right - rect.left

int run_message_loop();

HWND hMainWnd;

WNDPROC mainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iShowCmd) {
	vec2i wndSize = (vec2i){ 800, 600 };

	WNDCLASS wndClass = { 0 };
	wndClass.lpfnWndProc = mainWndProc ;
	wndClass.hInstance = hInstance;
	wndClass.lpszClassName = L"Peach";
	wndClass.hCursor = NULL;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = GetSysColorBrush(WHITE_BRUSH);
	wndClass.hIcon = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.style = NULL;

	RegisterClass(&wndClass);
	hMainWnd = CreateWindowEx(NULL, L"Peach", L"Bruh", WS_OVERLAPPEDWINDOW, 300, 200, 800, 600, NULL, NULL, hInstance, NULL);

	text_init(hMainWnd);

	//hMainWnd = create_independent_opengl_window(L"PeachTea", NULL, NULL, &wndClass);
	/*
	HDC hdc = GetDC(hMainWnd);
	RECT rect;
	GetWindowRect(hMainWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	renderer_resize(width, height);
	HGLRC hrc = create_rc_from_window(&hdc);
	wglMakeCurrent(hdc, hrc);
	GLEInit();
	

	int w = 40;
	MARGINS m = { w, w, w, w };
	//HRESULT hr = DwmExtendFrameIntoClientArea(hMainWnd, &m);
	//blurwnd(hMainWnd);
	*/

    UpdateWindow(hMainWnd);
	ShowWindow(hMainWnd, iShowCmd);

	int exitCode = run_message_loop();

	text_close();

	//ReleaseDC(hMainWnd, hdc);

	return exitCode;
}

int run_message_loop() {
	MSG msg;
	HDC hdc;

	while (1) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				return msg.wParam;
			}
		}

		hdc = GetDC(hMainWnd);
		//renderer_display(hdc);
		ReleaseDC(hMainWnd, hdc);
	}
}


//
//  Main WinProc.
//
WNDPROC mainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static PAINTSTRUCT ps;
	static HDC hdc;
	static HRESULT hr;
	static LRESULT result = 0;
	static RECT rect;

	switch (message)
	{
	case WM_SIZE:
		GetWindowRect(hMainWnd, &rect);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_RIGHT:
			move_caret(1, 0);
			break;
		case VK_LEFT:
			move_caret(-1, 0);
			break;
		case VK_UP:
			move_caret(0, -1);
			break;
		case VK_DOWN:
			move_caret(0, 1);
			break;
		}
		break;
	case WM_CHAR: 
		GetClientRect(hMainWnd, &rect);
		if (wParam == '\r') {
			char_typed('\n');
		}
		else {
			char_typed(wParam);
		}
		InvalidateRect(hMainWnd, &rect, TRUE);
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hMainWnd, &rect);

		render_text(hdc, rect);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SETFOCUS:
		;
		TEXTMETRIC tm;
		hdc = GetDC(hMainWnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hMainWnd, hdc);
		int lineHeight = tm.tmHeight;

		CreateCaret(hMainWnd, NULL, 0, lineHeight);
		break;
	case WM_KILLFOCUS:
		DestroyCaret();
		break;
	case WM_DESTROY:
		PostQuitMessage(69);
		DestroyCaret();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}