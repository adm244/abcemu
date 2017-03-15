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

#include <stdio.h>
#include <stdlib.h>

#include "win32_conio.h"
#include "vm.h"

#define int32 __int32
#define uint  unsigned int
#define uchar unsigned char

static struct _vm vm;

inline void updflags_nz();
inline void updflags_nzc();
inline void updflags_nzcv();

int32 * readfile(char *filename)
{
  FILE *file;
  size_t size = 0;
  int32 *res = NULL;
  
  file = fopen(filename, "rb");
  
  if( file != NULL ){
    while( !feof(file) ){
      size += BUFSIZ;
      res = (int32 *)realloc(res, size*sizeof(int32));
      
      if( res != NULL ){
        fread(res, sizeof(int32), BUFSIZ, file);
      } else{
        printf("Unable to allocate %d bytes of memory\n", size*sizeof(int32));
        break;
      }
    }
    fclose(file);
  } else{
    printf("Unable to read file \"%s\"\n", filename);
  }
  
  return(res);
}

int main(int argc, char *argv[])
{
  uint running = 0x1;
  
  //TODO(adm244): write a console arguments parser
  //vm.mem   = readfile("test.mem");
  //vm.code  = readfile("test.abc");
  vm.mem   = readfile(argv[1]);
  vm.code  = readfile(argv[2]);
  vm.stack = (int32 *)malloc(STACKSIZE);
  
  vm.r[0xE] = -1;
  vm.r[0xF] = 0;
  
  if( (vm.mem == NULL) || (vm.code == NULL) ){
    return;
  }
  
  while(running){
    int32 inst = vm.code[ vm.r[0xF]++ ];
    
    switch(inst){
      case ILDR:{
        int32 reg = vm.code[ vm.r[0xF]++ ];
        int32 val = vm.code[ vm.r[0xF]++ ];
        
        vm.r[reg] = val;
      } break;
      
      case ILDRM:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 msrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] = vm.mem[msrc];
      } break;
      
      case ILDMR:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] = vm.mem[ vm.r[rsrc] ];
      } break;
      
      case ISTR:{
        int32 mdest = vm.code[ vm.r[0xF]++ ];
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.mem[mdest] = vm.r[rsrc];
      } break;
      
      case IMOV:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] = vm.r[rsrc];
      } break;
      
      #define MATHOP(OPCODE, OP)\
      case OPCODE:{\
        int32 rdest = vm.code[ vm.r[0xF]++ ];\
        int32 rsrc = vm.code[ vm.r[0xF]++ ];\
        \
        vm.r[rdest] OP= vm.r[rsrc];\
      } break;
      
      MATHOP(IADD, +)
      MATHOP(ISUB, -)
      MATHOP(IMUL, *)
      MATHOP(IDIV, /)
      #undef MATHOP
      
      #define MATHOPS(OPCODE, OP)\
      case OPCODE:{\
        int32 rdest = vm.code[ vm.r[0xF]++ ];\
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];\
        \
        vm.r[rdest] OP= vm.r[rsrc];\
        updflags_nzcv();\
      } break;
      
      MATHOPS(IADDS, +)
      MATHOPS(ISUBS, -)
      MATHOPS(IMULS, *)
      MATHOPS(IDIVS, /)
      #undef MATHOPS
      
      case IPUSH:{
        int32 reg = vm.code[ vm.r[0xF]++ ];
        int32 sp = ++vm.r[0xE];
        
        vm.stack[sp] = vm.r[reg];
      } break;
      
      case IPOP:{
        int32 reg = vm.code[ vm.r[0xF]++ ];
        int32 sp = vm.r[0xE]--;
        
        vm.r[reg] = vm.stack[sp];
      } break;
      
      #define LOGOP(OPCODE, OP)\
      case OPCODE:{\
        int32 rdest = vm.code[ vm.r[0xF]++ ];\
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];\
        \
        vm.r[rdest] = vm.r[rdest] OP vm.r[rsrc];\
        updflags_nz();\
      } break;
      
      LOGOP(IANDS, &)
      LOGOP(IEORS, ^)
      LOGOP(IORRS, |)
      LOGOP(IBICS, & ~)
      #undef LOGOP
      
      case ITST:{
        int32 op1 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        int32 op2 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        
        _asm{
          mov  eax, op1
          test eax, op2
        }
        updflags_nz();
      } break;
      
      case ICMP:{
        int32 op1 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        int32 op2 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        
        _asm{
          mov eax, op1
          cmp eax, op2
        }
        updflags_nzcv();
      } break;
      
      case ICMPI:{
        int32 op1 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        int32 op2 = vm.code[ vm.r[0xF]++ ];
        
        _asm{
          mov eax, op1
          cmp eax, op2
        }
        updflags_nzcv();
      } break;
      
      case ICMN:{
        int32 op1 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        int32 op2 = vm.r[ vm.code[ vm.r[0xF]++ ] ];
        
        _asm{
          mov eax, op1
          add eax, op2
        }
        updflags_nzcv();
      } break;
      
      case ILSLS:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 shift = vm.code[ vm.r[0xF]++ ];
        int32 oprnd = vm.r[rdest];
        
        _asm{
          mov eax, oprnd
          mov ecx, shift
          sal eax, cl
          
          lea ecx, vm.r
          mov ebx, rdest
          mov [ecx+4*ebx], eax
        }
        updflags_nzc();
      } break;
      
      case ILSRS:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 shift = vm.code[ vm.r[0xF]++ ];
        int32 oprnd = vm.r[rdest];
        
        _asm{
          mov eax, oprnd
          mov ecx, shift
          sar eax, cl
          
          lea ecx, vm.r
          mov ebx, rdest
          mov [ecx+4*ebx], eax
        }
        updflags_nzc();
      } break;
      
      case INEGS:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] *= -1;
        updflags_nzcv();
      } break;
      
      case IMVNS:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] = ~vm.r[rdest];
        updflags_nz();
      } break;
      
      case IB:{
        vm.r[0xF] = vm.code[ vm.r[0xF] ];
      } break;
      
      case IBEQ:{
        if( vm.flags & 0x40000000 ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      case IBNE:{
        if( vm.flags & 0x40000000 ){
          vm.r[0xF]++;
        } else{
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        }
      } break;
      
      case IBMI:{
        if( vm.flags & 0x80000000 ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      case IBPL:{
        if( vm.flags & 0x80000000 ){
          vm.r[0xF]++;
        } else{
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        }
      } break;
      
      //TODO(adm244): V flag should be taken into account as well
      //NOTE(adm244): page 393 of Intel architecture manual
      
      //N xor V = 0
      case IBGE:{
        if( (vm.flags & 0x40000000) ||
           !(vm.flags & 0x80000000) ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      //N xor V = 1
      case IBLT:{
        if( !(vm.flags & 0x40000000) &&
             (vm.flags & 0x80000000) ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      //(N xor V) or Z = 0
      case IBGT:{
        if( !(vm.flags & 0x40000000) &&
            !(vm.flags & 0x80000000) ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      //(N xor V) or Z = 1
      case IBLE:{
        if( (vm.flags & 0x40000000) ||
            (vm.flags & 0x80000000) ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      case IBCS:{
        if( vm.flags & 0x20000000 ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      case IBCC:{
        if( vm.flags & 0x20000000 ){
          vm.r[0xF]++;
        } else{
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        }
      } break;
      
      case IBVS:{
        if( vm.flags & 0x10000000 ){
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      case IBVC:{
        if( vm.flags & 0x10000000 ){
          vm.r[0xF]++;
        } else{
          vm.r[0xF] = vm.code[ vm.r[0xF] ];
        }
      } break;
      
      case ISVC:{
        int32 call = vm.code[ vm.r[0xF]++ ];
        int32 proc = vm.r[0x4];
        
        switch(call){
          case SVC_OS:{
            switch(proc){
              case PROC_PUTC:{
                uchar ch = (uchar)vm.r[0x0];
                
                putch(ch);
              } break;
              
              case PROC_PUTCXY:{
                uchar ch = (uchar)vm.r[0x2];
                short x = (short)vm.r[0x0];
                short y = (short)vm.r[0x1];
                
                putchxy(ch, x, y);
              } break;
              
              case PROC_GETC:{
                uchar ch;
                
                getch(&ch);
                vm.r[0x0] = (int32)ch;
              } break;
              
              case PROC_GETCXY:{
                uchar ch;
                short x = (short)vm.r[0x0];
                short y = (short)vm.r[0x1];
                
                getchxy(&ch, x, y);
                vm.r[0x0] = (int32)ch;
              } break;
              
              case PROC_SETCUR:{
                short x = (short)vm.r[0x0];
                short y = (short)vm.r[0x1];
                
                gotoxy(x, y);
              } break;
              
              case PROC_GETCUR:{
                short x;
                short y;
                
                getxy(&x, &y);
                vm.r[0x0] = (int32)x;
                vm.r[0x1] = (int32)y;
              } break;
              
              case PROC_CLSSCR:{
                clsscr();
              } break;
              
              case PROC_GETKEY:{
                uchar keycode;
                uchar scancode;
                short state;
                
                getkey(&keycode, &scancode, &state);
                vm.r[0x0] = (int32)keycode;
                vm.r[0x1] = (int32)scancode;
                vm.r[0x2] = (int32)state;
              } break;
              
              case PROC_GETCP:{
                uint incp;
                uint outcp;
                
                getcodepage(&incp, &outcp);
                vm.r[0x0] = (int32)incp;
                vm.r[0x1] = (int32)outcp;
              } break;
              
              case PROC_SETCP:{
                uint incp = (uint)vm.r[0x0];
                uint outcp = (uint)vm.r[0x1];
                
                setcodepage(incp, outcp);
              } break;
              
              case PROC_GETCS:{
                short width;
                short height;
                
                getconsize(&width, &height);
                vm.r[0x0] = (int32)width;
                vm.r[0x1] = (int32)height;
              } break;
              
              case PROC_CURVIS:{
                int state = vm.r[0x0];
                setcurstate(state);
              } break;
              
              case PROC_EXIT:{
                running = 0x0;
              } break;
            }
          } break;
        }
      } break;
    }
    
  }
  
  return(0);
}

inline void updflags_nz()
{
  _asm{
    mov ebx, vm.flags
    
    lahf //stores the low byte of the flags into ah
    
    jns nclear
    or ebx, 0x80000000
    jmp nend
    nclear: and ebx, 0x7fffffff
    nend:
    
    sahf //restores flags from ah register
    
    jnz zclear
    or ebx, 0x40000000
    jmp zend
    zclear: and ebx, 0xbfffffff
    zend:
    
    mov vm.flags, ebx
  }
}

inline void updflags_nzc()
{
  _asm{
    mov ebx, vm.flags
    
    lahf
    
    jns nclear
    or ebx, 0x80000000
    jmp nend
    nclear: and ebx, 0x7fffffff
    nend:
    
    sahf
    
    jnz zclear
    or ebx, 0x40000000
    jmp zend
    zclear: and ebx, 0xbfffffff
    zend:
    
    sahf
    
    jnc cclear
    or ebx, 0x20000000
    jmp cend
    cclear: and ebx, 0xdfffffff
    cend:
    
    mov vm.flags, ebx
  }
}

inline void updflags_nzcv()
{
  _asm{
    mov ebx, vm.flags
    
    lahf
    
    jno vclear
    or ebx, 0x10000000
    jmp vend
    vclear: and ebx, 0xefffffff
    vend:
    
    sahf
    
    jns nclear
    or ebx, 0x80000000
    jmp nend
    nclear: and ebx, 0x7fffffff
    nend:
    
    sahf
    
    jnz zclear
    or ebx, 0x40000000
    jmp zend
    zclear: and ebx, 0xbfffffff
    zend:
    
    sahf
    
    jnc cclear
    or ebx, 0x20000000
    jmp cend
    cclear: and ebx, 0xdfffffff
    cend:
    
    mov vm.flags, ebx
  }
}
