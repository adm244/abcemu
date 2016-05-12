/*
REGISTERS: 16 32-bit registers
  r0-r13 - general purpose
  r14 - stack pointer (SP)
  r15 - program counter (PC)
  
  [   ALU   ] [      RESERVED     ] [    CONTROL BITS     ]
  31 30 29 28 27 26 25 24 23 ... 08 07 06 05 04 03 02 01 00
  N  Z  C  V  ?? ?? ?? ?? ?? ... ?? I  F  T  M4 M3 M2 M1 M0
  
  N - negative (less)
  Z - zero
  C - carry
  V - overflow
  
INSTRUCTIONS:
  (IMPLEMENTED) LDR - loads constant into register
  (IMPLEMENTED) LDRM - loads value from memory into register
  (IMPLEMENTED) STR - stores register value into memory
  (IMPLEMENTED) MOV - moves value from register to register
  (IMPLEMENTED) ADD - adds value of register to register
  (IMPLEMENTED) ADDS - adds value of register to register (N, Z, C, V)
  (IMPLEMENTED) SUB - subs value of register from register
  (IMPLEMENTED) SUBS - subs value of register from register (N, Z, C, V)
  (IMPLEMENTED) MUL - multiplies register value by register value
  (IMPLEMENTED) MULS - multiplies register value by register value (N, Z, C, V)
  (IMPLEMENTED) DIV - divides register value by register value
  (IMPLEMENTED) DIVS - divides register value by register value (N, Z, C, V)
  (IMPLEMENTED) PUSH - pushes value of register on top of a stack
  (IMPLEMENTED) POP - pops value from top of a stack into register
  (IMPLEMENTED) SVC - interrupt call to operating system (HACKED)
  (IMPLEMENTED) ANDS - logical and between register values (N, Z)
  (IMPLEMENTED) EORS - exclusive or between register values (N, Z)
  (IMPLEMENTED) ORRS - logical or between register values (N, Z)
  (IMPLEMENTED) BICS - logical and between register value and !(register value) (N, Z)
  (IMPLEMENTED) TST - updates flags based on result of AND operation (N, Z)
  (IMPLEMENTED) LSLS - logical shift left << and update flags (N, Z, C)
  (IMPLEMENTED) LSRS - logical shift right >> and update flags (N, Z, C)
  (IMPLEMENTED) CMP - updates flags based on result of SUB operation (N, Z, C, V)
  (IMPLEMENTED) CMN - updates flags based on result of ADD operation (N, Z, C, V)
  (IMPLEMENTED) B - unconditional jump
  (IMPLEMENTED) BEQ - jump if equal
  (IMPLEMENTED) BNE - jump if not equal
  (IMPLEMENTED) BCS - jump if carry is set
  (IMPLEMENTED) BCC - jump if carry is not set
  (IMPLEMENTED) BMI - jump if negative
  (IMPLEMENTED) BPL - jump if positive or zero
  (IMPLEMENTED) BVS - jump if overflow
  (IMPLEMENTED) BVC - jump if no overflow
  (IMPLEMENTED) BGE - jump if greater than or equal
  (IMPLEMENTED) BLT - jump if less than
  (IMPLEMENTED) BGT - jump if greater than
  (IMPLEMENTED) BLE - jump if less than or equal
  (IMPLEMENTED) NEGS - changes sign of a register value (N, Z, C, V)
  (IMPLEMENTED) MVNS - bitwise not of a register value (N, Z)
  (IMPLEMENTED) NOP - does nothing

PROCEDURES (HACKED):
  OS supervisor calls:
  
  (IMPLEMENTED) PUTC - writes character from r0 to a console at current position and moves a carret
  (IMPLEMENTED) PUTCXY - writes character from r2 to a console at x = r0 and y = r1 (x, y are lower 8 bits)
  (IMPLEMENTED) GETC - reads character from a console at current position and stores it in r0 and moves a carret
  (IMPLEMENTED) GETCXY - reads character from a console at x = r0 and y = r1 and stores it in r0 (x, y are lower 8 bits)
  (IMPLEMENTED) SETCUR - sets a console cursor position at x = r0 and y = r1 (x, y are lower 8 bits)
  (IMPLEMENTED) GETCUR - gets a console cursor position and stores it in r0 = x and r1 = y
  (IMPLEMENTED) GETKEY - check for a keypress and stores a keycode in r0, scancode in r1 and state in r2 (non 0 - released, 0 - pressed)
  (IMPLEMENTED) CLSSCR - clears a console area (no attributes applied)
  (IMPLEMENTED) CURVIS - sets visibility of a console cursor based on value of r0 (0 - hidden, 1 - visible)
  (IMPLEMENTED) GETCP - gets an id of a code page that a console uses and stores input cp in r0 and output cp in r1
  (IMPLEMENTED) SETCP - sets a code page for a console with id = r0 for input cp and id = r1 for output cp
  (IMPLEMENTED) GETCS - gets a console size and stores width in r0 and height in r1 (not zero based!)
  (IMPLEMENTED) EXIT - successefully terminates a program
*/

#include <stdio.h>
#include <stdlib.h>

#include "win32_conio.h"

#define int32 __int32
#define uint  unsigned int
#define uchar unsigned char

#define MEMSIZE   1024 //256 bytes
#define STACKSIZE 1024

#define INOP   0x00
#define ILDR   0x01
#define IMOV   0x02
#define IADD   0x03
#define IADDS  0x04
#define ISUB   0x05
#define ISUBS  0x06
#define IMUL   0x07
#define IMULS  0x08
#define IDIV   0x09
#define IDIVS  0x0A
#define IPUSH  0x0B
#define IPOP   0x0C
#define ISVC   0x0D
#define IB     0x0E
#define ISTR   0x0F
#define ILDRM  0x10
#define IANDS  0x11
#define IEORS  0x12
#define IORRS  0x13
#define IBICS  0x14
#define IBEQ   0x15
#define IBNE   0x16
#define IBMI   0x17
#define IBPL   0x18
#define IBGE   0x19
#define IBLT   0x1A
#define IBGT   0x1B
#define IBLE   0x1C
#define ITST   0x1D
#define ICMP   0x1E
#define ICMN   0x1F
#define IBCS   0x20
#define IBCC   0x21
#define IBVS   0x22
#define IBVC   0x23
#define ILSLS  0x24
#define ILSRS  0x25
#define INEGS  0x26
#define IMVNS  0x27

#define SVC_OS 0x10

#define PROC_EXIT   0x01
#define PROC_PUTC   0x02
#define PROC_PUTCXY 0x03
#define PROC_GETC   0x04
#define PROC_GETCXY 0x05
#define PROC_SETCUR 0x06
#define PROC_GETCUR 0x07
#define PROC_CLSSCR 0x08
#define PROC_GETKEY 0x09
#define PROC_CURVIS 0x0A
#define PROC_GETCP  0x0B
#define PROC_SETCP  0x0C
#define PROC_GETCS  0x0D

struct _vm{
  int32 r[0x10];
  int32 flags;
  
  int32 *mem;
  int32 *code;
  int32 *stack;
} vm;

inline void updflags_nz();
inline void updflags_nzc();
inline void updflags_nzcv();

int main()
{
  int32 code[] = {
    /*ILDR, 0x0, 0x03,
    ILDR, 0x1, 0x01,
    ILDR, 0x4, PROC_SETCUR,
    ISVC, SVC_OS,
    ILDR, 0x4, PROC_GETC,
    ISVC, SVC_OS,
    IPUSH, 0x0,
    ILDR, 0x0, 30,
    ILDR, 0x1, 16,
    ILDR, 0x4, PROC_SETCUR,
    ISVC, SVC_OS,
    IPOP, 0x0,
    ILDR, 0x4, PROC_PUTC,
    ISVC, SVC_OS,*/
    
    /*ILDR, 0x4, PROC_CLSSCR,
    ISVC, SVC_OS,
    ILDR, 0x0, 0x30,
    ILDR, 0x4, PROC_PUTC,
    ISVC, SVC_OS,*/
    
    //437 by default
    /*ILDR, 0x0, 866,
    IMOV, 0x1, 0x0,
    ILDR, 0x4, PROC_SETCP,
    ISVC, SVC_OS,
    ILDR, 0x4, PROC_GETCS,
    ISVC, SVC_OS,
    ILDR, 0x1, 1,
    ISUB, 0x0, 0x1,
    ILDR, 0x4, PROC_SETCUR,
    ISVC, SVC_OS,
    ILDR, 0x0, 0xA6,
    ILDR, 0x4, PROC_PUTC,
    ISVC, SVC_OS,*/
    
    ILDR, 0x4, PROC_GETKEY,
    ISVC, SVC_OS,
    
    ILDR, 0x5, 0x0D,
    ICMP, 0x0, 0x5,
    IBEQ, 15,
    IB, 0,
    
    ILDR, 0x4, PROC_EXIT,
    ISVC, SVC_OS
  };
  
  vm.mem   = (int32 *)malloc(MEMSIZE);
  vm.stack = (int32 *)malloc(STACKSIZE);
  vm.code  = code;
  
  vm.r[0xE] = -1;
  
  uint running = 0x1;
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
