#ifndef RENDERER_H
#define RENDERER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

void renderer_init();
void renderer_resize(int width, int height);
void renderer_display(HDC hdc);
void renderer_close();

#endif