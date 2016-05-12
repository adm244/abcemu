#include "win32_conio.h"
#include <stdio.h>

void gotoxy(short x, short y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  COORD pos = {x, y};
  SetConsoleCursorPosition(console, pos);
}

void getxy(short *x, short *y)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  GetConsoleScreenBufferInfo(console, &bufinfo);
  
  *x = bufinfo.dwCursorPosition.X;
  *y = bufinfo.dwCursorPosition.Y;
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
  
  if( bufinfo.dwCursorPosition.X < bufinfo.dwSize.X ){
    bufinfo.dwCursorPosition.X++;
  } else{
    bufinfo.dwCursorPosition.X = 0;
    bufinfo.dwCursorPosition.Y++;
  }
  
  SetConsoleCursorPosition(console, bufinfo.dwCursorPosition);
  
  *ch = buffer;
}

void putch(unsigned char ch)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD charswritten;
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  
  GetConsoleScreenBufferInfo(console, &bufinfo);
  WriteConsoleOutputCharacter(console, &ch, 1, bufinfo.dwCursorPosition, &charswritten);
  
  if( bufinfo.dwCursorPosition.X < bufinfo.dwSize.X-1 ){
    bufinfo.dwCursorPosition.X++;
  } else{
    bufinfo.dwCursorPosition.X = 0;
    bufinfo.dwCursorPosition.Y++;
  }
  
  SetConsoleCursorPosition(console, bufinfo.dwCursorPosition);
}

void getconsize(short *width, short *height)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  
  GetConsoleScreenBufferInfo(console, &bufinfo);
  *width  = bufinfo.dwSize.X;
  *height = bufinfo.dwSize.Y;
}

void clsscr()
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO bufinfo;
  COORD startpos = {0, 0};
  DWORD consize;
  DWORD charswritten;
  
  GetConsoleScreenBufferInfo(console, &bufinfo);
  consize = bufinfo.dwSize.X * bufinfo.dwSize.Y;
  
  FillConsoleOutputCharacter(console, (TCHAR)0x20, consize, startpos, &charswritten);
  //FillConsoleOutputAttribute(...);
  SetConsoleCursorPosition(console, startpos);
}

void setcurstate(int visible)
{
  HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_CURSOR_INFO curinfo;
  
  GetConsoleCursorInfo(console, &curinfo);
  curinfo.bVisible = visible == 0 ? FALSE : TRUE;
  
  SetConsoleCursorInfo(console, &curinfo);
}

void getcodepage(unsigned int *incp, unsigned int *outcp)
{
  *incp  = GetConsoleCP();
  *outcp = GetConsoleOutputCP();
}

void setcodepage(unsigned int incp, unsigned int outcp)
{
  SetConsoleCP(incp);
  SetConsoleOutputCP(outcp);
}

void getkey(unsigned char *keycode, unsigned char *scancode, short *state)
{
  HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
  INPUT_RECORD inputbuf[10];
  DWORD eventsread;
  short i;
  
  ReadConsoleInput(console, inputbuf, 10, &eventsread);
  
  for( i = eventsread-1; i >= 0; i-- ){
    KEY_EVENT_RECORD keyinfo;
    
    if( inputbuf[i].EventType == KEY_EVENT ){
      keyinfo = inputbuf[i].Event.KeyEvent;
      *state = keyinfo.bKeyDown;
      *keycode  = keyinfo.wVirtualKeyCode;
      *scancode = keyinfo.wVirtualScanCode;
    }
  }
}
