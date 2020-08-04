#define _CRT_SECURE_NO_WARNINGS

#include "text_editor.h"

#include <stdlib.h>
#include <stdio.h>

#include "PeachTea/PeachTea.h"

char* str;
char* temp;
int totalStringLength;
int usedStringLength;

lines_data* pLD = NULL;
int caretPos = 0;

int lastXOffset = 0; // cache the intended x offset. If you hit up/down arrow from a char that is 10 characters out, 
//                       try to persist that spacing unless the user types/moves left/right
int currentXOffset = 0;
int currentLine = 0;

HWND hMainWnd;


void update_caret_pos(HDC hdc) {
	static TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	int lineHeight = tm.tmHeight + LINE_PADDING;
	

	char* s1 = calloc(50, sizeof(char));
	sprintf(s1, "%i: (%i, %i)", usedStringLength, currentXOffset, currentLine);
	TextOutA(hdc, 0, 370, s1, strlen(s1));
	free(s1);

	if (pLD != NULL && pLD->numLines > 0) {
		char* line = *(pLD->lines + currentLine);

		int tabWidth = 8 * tm.tmAveCharWidth;
		int numTabPositions = 20;
		int* tabPositions = calloc(numTabPositions, sizeof(int));
		for (int i = 0; i < numTabPositions; i++) {
			*(tabPositions + i) = tabWidth * i;
		}

		DWORD textExtent = GetTabbedTextExtentA(hdc, line, currentXOffset, numTabPositions, tabPositions);
		int lineWidth = LOWORD(textExtent);

		free(tabPositions);


		char* s = calloc(50, sizeof(char));
		sprintf(s, "%i, %i", lineWidth, lineHeight * (currentLine));
		TextOutA(hdc, 0, 400, s, strlen(s));
		free(s);

		
		SetCaretPos(lineWidth, lineHeight * (currentLine));

		if (line != NULL) {
			char c = *(line + currentXOffset - 1);
			if (c == '\n') {
				SetCaretPos(0, lineHeight * (currentLine + 1));
			}
		}
	}
	else {
		SetCaretPos(0, 0);
		TextOutA(hdc, 0, 400, "0, 0", 4);
	}

	ShowCaret(hMainWnd);
}

void text_init(HWND hWnd) {
	hMainWnd = hWnd;

	pLD = calloc(1, sizeof(lines_data));

	usedStringLength = 0;
	totalStringLength = 50;
	str = calloc(totalStringLength, sizeof(char));
	temp = calloc(totalStringLength, sizeof(char));
}

void text_close() {
	free(str);
	free(temp);

	free_lines_data(pLD);
	free(pLD);
}

vec2i get_line_offset_positions(int pos) {
	int charIndex = 0;
	for (int i = 0; i < pLD->numLines; i++) {
		int len = *(pLD->lengths + i);
		if (pos > charIndex && pos <= charIndex + len) {
			return (vec2i) { i, pos - charIndex };
		}

		charIndex += len;
	}

	return (vec2i) { 0, 0 };
}


lines_data get_str_lines() {
	lines_data ld = { 0 };

	ld.numLines = 1;
	for (int i = 0; i < usedStringLength; i++) {
		if (*(str + i) == '\n') {
			ld.numLines++;
		}
	}

	int lastLineIndex = -1;
	ld.lines = calloc(ld.numLines, sizeof(char*));
	ld.lengths = calloc(ld.numLines, sizeof(int));
	int lineIndex = 0;

	for (int i = 0; i < usedStringLength; i++) {
		if (*(str + i) == '\n' || i == usedStringLength - 1) {
			int len = i - lastLineIndex;

			char* line = calloc(len + 1, sizeof(char));
			memcpy(line, str + lastLineIndex + 1, len);

			*(ld.lines + lineIndex) = line;
			*(ld.lengths + lineIndex) = len;

			lastLineIndex = i;
			lineIndex++;
		}
	}

	return ld;
}

void insert_char(char c, int pos) {
	// add char to str
	if (c == '\b') { // instead of adding this character, remove the character at the current pos
		if (usedStringLength > 0 && pos > 0) {
			memcpy(temp, str, usedStringLength * sizeof(char));

			memcpy(str, temp + pos + 1, usedStringLength - pos);

			usedStringLength--;
		}
	}
	else {
		if (usedStringLength + 1 >= totalStringLength) {
			totalStringLength *= 2;

			str = realloc(str, totalStringLength * sizeof(char));
			temp = realloc(temp, totalStringLength * sizeof(char));
		}

		memcpy(temp, str, usedStringLength * sizeof(char));

		*(str + pos) = c;
		memcpy(str, temp + pos, (usedStringLength - pos) * sizeof(char));

		usedStringLength++;
	}

	// update lines data
	if (pLD != NULL) {
		free_lines_data(pLD);
	}

	*pLD = get_str_lines();

	// update the current line and offset data
	vec2i strPositions;
	if (c != '\b') {
		strPositions = get_line_offset_positions(pos + 1);
	}
	else {
		strPositions = get_line_offset_positions(max(0, pos - 1));
	}

	currentLine = strPositions.x;
	lastXOffset = currentXOffset = strPositions.y;
}

void char_typed(char c) {
	int index = 0;

	if (pLD != NULL) {
		int charIndex = 0;
		for (int i = 0; i < pLD->numLines; i++) {
			int len = *(pLD->lengths + i);
			
			if (i == currentLine) {
				index = charIndex + lastXOffset;
				break;
			}

			charIndex += len;
		}
	}

	insert_char(c, index);
}

void render_text(HDC hdc, RECT rect) {
	if (pLD != NULL) {
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));

		static TEXTMETRIC tm;
		GetTextMetrics(hdc, &tm);
		int lineHeight = tm.tmHeight + LINE_PADDING;

		for (int i = 0; i < pLD->numLines; i++) {
			RECT lineRect = { 0 };
			lineRect.left = rect.left;
			lineRect.right = rect.right;
			lineRect.top = rect.top + i * lineHeight;
			lineRect.bottom = rect.top + (i + 1) * lineHeight;

			char* line = *(pLD->lines + i);
			int len = *(pLD->lengths + i);

			DrawTextA(hdc, line, len, &lineRect, DT_LEFT | DT_EXPANDTABS);
		}
	}

	update_caret_pos(hdc);
	ShowCaret(hMainWnd);
}

void move_caret(int deltaCharX, int deltaCharY) {
	if (abs(deltaCharX) > 0) {
		caretPos += deltaCharX;

		int xOffset = 0;
		int charIndex = caretPos;
		char c = 0;

		while (charIndex > 1 && c != '\n') {
			charIndex--;
			c = *(str + charIndex);
			xOffset++;
		}

		currentXOffset = xOffset;
		lastXOffset = xOffset;
	}

	if (abs(deltaCharY) > 0) {
		if (currentLine + deltaCharY >= 0 && currentLine + deltaCharY < pLD->numLines) {
			currentLine += deltaCharY;
		}

		int len = *(pLD->lengths + currentLine);
		currentXOffset = min(len - 1, lastXOffset);
	}

	HDC hdc = GetDC(hMainWnd);
	update_caret_pos(hdc);
	ReleaseDC(hMainWnd, hdc);
}
