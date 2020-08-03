#include "renderer.h"
#include "PeachTea/PeachTea.h"

GLuint blurProg;

void renderer_init() {
	GLuint bvs = create_vertex_shader("blur.vs");
	GLuint bfs = create_fragment_shader("blur.fs");
	GLuint bshaders[] = { bvs, bfs };
	blurProg = create_program(bshaders, 2);


}

void renderer_resize(int width, int height) {
	glViewport(0, 0, width, height);
}

HDC screenshot(RECT r) {
	HDC     hScreen = GetDC(HWND_DESKTOP);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, r.right - r.left, r.bottom - r.top);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, r.right - r.left, r.bottom - r.top, hScreen, r.left, r.top, SRCCOPY);

	return hBitmap;
}

void renderer_display(HDC hdc) {
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	HWND hWnd = WindowFromDC(hdc);
	RECT r;
	GetWindowRect(hWnd, &r);
	HDC background = GetDC(HWND_DESKTOP);
	BitBlt(hdc, 0, 0, r.right - r.left, r.bottom - r.top, background, r.left, r.top, SRCCOPY);
	ReleaseDC(HWND_DESKTOP, background);
	//SwapBuffers(hdc);
}

void renderer_close() {
	glDeleteProgram(blurProg);
}