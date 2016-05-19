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
