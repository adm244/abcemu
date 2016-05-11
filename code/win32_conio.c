#include "win32_conio.h"

void getxy(short *x, short *y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  GetConsoleScreenBufferInfo(console, &bufinfo);
  
  *x = bufinfo.dwCursorPosition.X;
  *y = bufinfo.dwCursorPosition.Y;
}

void gotoxy(short x, short y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD pos = {x, y};
  SetConsoleCursorPosition(console, pos);
}

void getchxy(unsigned char *ch, short x, short y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  UCHAR buffer;
  COORD pos = {x, y};
  DWORD charsread;
  
  ReadConsoleOutputCharacter(console, &buffer, 1, pos, &charsread);
  
  *ch = buffer;
}

void putchxy(unsigned char ch, short x, short y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD pos = {x, y};
  DWORD charswritten;
  
  WriteConsoleOutputCharacter(console, &ch, 1, pos, &charswritten);
}

void getch(unsigned char *ch)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  UCHAR buffer;
  DWORD charsread;
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  
  GetConsoleScreenBufferInfo(console, &bufinfo);
  ReadConsoleOutputCharacter(console, &buffer, 1, bufinfo.dwCursorPosition, &charsread);
  
  *ch = buffer;
}

void putch(unsigned char ch)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD charswritten;
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  
  GetConsoleScreenBufferInfo(console, &bufinfo);
  WriteConsoleOutputCharacter(console, &ch, 1, bufinfo.dwCursorPosition, &charswritten);
}
