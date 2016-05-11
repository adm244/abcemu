#ifndef WIN32_CONIO
#define WIN32_CONIO

#include <windows.h>

void  getxy(short *x, short *y);
void gotoxy(short  x, short  y);

void getchxy(unsigned char *ch, short x, short y);
void putchxy(unsigned char ch, short x, short y);

void getch(unsigned char *ch);
void putch(unsigned char ch);

#endif
