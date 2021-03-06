/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WIN32_CONIO
#define WIN32_CONIO

#include <windows.h>

void gotoxy(short x, short y);
void getxy(short *x, short *y);

void getchxy(unsigned char *ch, short x, short y);
void putchxy(unsigned char ch, short x, short y);

void getch(unsigned char *ch);
void putch(unsigned char ch);

void getconsize(short *width, short *height);

void printstr(unsigned char *str);

void clsscr();
void setcurstate(int visible);

void getcodepage(unsigned int *incp, unsigned int *outcp);
void setcodepage(unsigned int incp, unsigned int outcp);

void getkey(unsigned char *keycode, unsigned char *scancode, short *state);

#endif
