#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define LINE_PADDING 2;

void text_init(HWND hMainWnd);

void char_typed(char c);

void move_caret(int deltaCharX, int deltaCharY);

void text_close();