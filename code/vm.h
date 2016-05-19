/*
  vm.h: contains information about vm

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
  (IMPLEMENTED) LDMR - loads value from memory address pointed by register
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
  (IMPLEMENTED) CMPI - same as CMP but op2 is a constant
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
  (IMPLEMENTED) GETKEY - checks for a keypress and stores a keycode in r0, scancode in r1 and state in r2 (non 0 - released, 0 - pressed)
  (IMPLEMENTED) CLSSCR - clears a console area (no attributes applied)
  (IMPLEMENTED) CURVIS - sets visibility of a console cursor based on value of r0 (0 - hidden, 1 - visible)
  (IMPLEMENTED) GETCP - gets an id of a code page that a console uses and stores input cp in r0 and output cp in r1
  (IMPLEMENTED) SETCP - sets a code page for a console with id = r0 for input cp and id = r1 for output cp
  (IMPLEMENTED) GETCS - gets a console size and stores width in r0 and height in r1 (not zero based!)
  (IMPLEMENTED) EXIT - successefully terminates a program
*/

#ifndef _VM_H_
#define _VM_H_

#define int32 __int32
#define uint  unsigned int
#define uchar unsigned char

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
#define ILDMR  0x28
#define ICMPI  0x29

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

#define MEMSIZE   1024 //256 bytes
#define STACKSIZE 1024

struct _vm{
  int32 r[0x10];
  int32 flags;
  
  int32 *mem;
  int32 *code;
  int32 *stack;
};

#endif
