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
  (IMPLEMENTED) SUB - subs value of register from register
  (IMPLEMENTED) MUL - multiplies register value by register value
  (IMPLEMENTED) DIV - divides register value by register value
  (IMPLEMENTED) PUSH - pushes value of register on top of a stack
  (IMPLEMENTED) POP - pops value from top of a stack into register
  (IMPLEMENTED) SVC - interrupt call to operating system (HACKED)
  (IMPLEMENTED) ANDS - logical and between register values (N, Z)
  (IMPLEMENTED) EORS - exclusive or between register values (N, Z)
  (IMPLEMENTED) ORRS - logical or between register values (N, Z)
  (IMPLEMENTED) BICS - logical and between register value and !(register value) (N, Z)
  TST - updates flags based on result of AND operation (N, Z)
  LSLS - logical shift left << and update flags (N, Z, C)
  LSRS - logical shift right >> and update flags (N, Z, C)
  CMP - updates flags based on result of SUB operation (N, Z, C, V)
  CMN - updates flags based on result of ADD operation (N, Z, C, V)
  (IMPLEMENTED) B - unconditional jump
  (IMPLEMENTED) BEQ - jump if equal
  (IMPLEMENTED) BNE - jump if not equal
  BCS - jump if carry is set
  BCC - jump if carry is not set
  (IMPLEMENTED) BMI - jump if negative
  (IMPLEMENTED) BPL - jump if positive or zero
  BVS - jump if overflow
  BVC - jump if no overflow
  BGE - jump if greater than or equal
  BLT - jump if less than
  BGT - jump if greater than
  BLE - jump if less than or equal
  (IMPLEMENTED) NOP - does nothing
*/

#include <stdio.h>
#include <stdlib.h>

#define int32 int
#define uint  unsigned int

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

#define PROC_EXIT  0x10
#define PROC_PRINT 0x11

struct _vm{
  int32 r[0x10];
  int32 flags;
  
  int32 *mem;
  int32 *code;
  int32 *stack;
} vm;

int main()
{
  int32 code[] = {
    /*ILDR, 0x1, 0x30,
    IMOV, 0x0, 0x1,
    IB, 0x13,
    ISVC, PROC_PRINT,
    IPUSH, 0x0,
    IADD, 0x0, 0x01,
    ISVC, PROC_PRINT,
    IPOP, 0x0,
    ISVC, PROC_PRINT,*/
    
    /*ILDR, 0x5, 0x31,
    ISTR, 0x41, 0x5,
    ILDRM, 0x0, 0x41,
    ISVC, PROC_PRINT,*/
    
    ILDR, 0x0, 0x31,
    ILDR, 0x2, 0xFF000000,
    ILDR, 0x1, 0x40000000,
    INOP,
    IBICS, 0x2, 0x1,
    IBPL, 0x11,
    ISVC, PROC_PRINT,
    ISVC, PROC_EXIT
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
      
      //STR - stores register value into memory
      case ISTR:{
        int32 mdest = vm.code[ vm.r[0xF]++ ];
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.mem[mdest] = vm.r[rsrc];
      } break;
      
      //LDRM - loads value from memory into register
      case ILDRM:{
        int32 rdest = vm.code[ vm.r[0xF]++ ];
        int32 msrc  = vm.code[ vm.r[0xF]++ ];
        
        vm.r[rdest] = vm.mem[msrc];
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
      
      //TODO(adm244): MATHOPs with flags changes
      
      case IPUSH:{
        int32 reg = vm.code[ vm.r[0xF]++ ];
        int32 sp = ++vm.r[0xE];
        
        vm.stack[sp] = vm.r[reg];
      } break;
      
      case IPOP:{
        int32 reg = vm.code[ vm.r[0xF]++ ];
        int32 sp = vm.r[0xE]--;
        int32 val = vm.stack[sp];
        
        vm.r[reg] = val;
      } break;
      
      #define LOGOP(OPCODE, OP)\
      case OPCODE:{\
        int32 rdest = vm.code[ vm.r[0xF]++ ];\
        int32 rsrc  = vm.code[ vm.r[0xF]++ ];\
        \
        vm.r[rdest] = vm.r[rdest] OP vm.r[rsrc];\
        \
        if( vm.r[rdest] & 0x80000000 ){\
          vm.flags = vm.flags | 0x80000000;\
        } else{\
          vm.flags = vm.flags & 0x7FFFFFFF;\
        }\
        \
        if( vm.r[rdest] == 0x0 ){\
          vm.flags = vm.flags | 0x40000000;\
        } else{\
          vm.flags = vm.flags & 0xBFFFFFFF;\
        }\
      } break;
      
      LOGOP(IANDS, &)
      LOGOP(IEORS, ^)
      LOGOP(IORRS, |)
      LOGOP(IBICS, & ~)
      #undef LOGOP
      
      case IB:{
        int32 pc = vm.code[ vm.r[0xF] ];
        vm.r[0xF] = pc;
      } break;
      
      //BEQ - jump if equal
      case IBEQ:{
        if( vm.flags & 0x40000000 ){
          int32 pc = vm.code[ vm.r[0xF]++ ];
          vm.r[0xF] = pc;
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      //BNE - jump if not equal
      case IBNE:{
        if( vm.flags & 0x40000000 ){
          vm.r[0xF]++;
        } else{
          int32 pc = vm.code[ vm.r[0xF]++ ];
          vm.r[0xF] = pc;
        }
      } break;
      
      //BMI - jump if negative
      case IBMI:{
        if( vm.flags & 0x80000000 ){
          int32 pc = vm.code[ vm.r[0xF]++ ];
          vm.r[0xF] = pc;
        } else{
          vm.r[0xF]++;
        }
      } break;
      
      //BPL - jump if positive or zero
      case IBPL:{
        if( vm.flags & 0x80000000 ){
          vm.r[0xF]++;
        } else{
          int32 pc = vm.code[ vm.r[0xF]++ ];
          vm.r[0xF] = pc;
        }
      } break;
      
      //TODO(adm244): implement those, look at N and Z flags
      /*
      //BGE - jump if greater than or equal
      case IBGE:{
        
      } break;
      
      //BLT - jump if less than
      case IBLT:{
        
      } break;
      
      //BGT - jump if greater than
      case IBGT:{
        
      } break;
      
      //BLE - jump if less than or equal
      case IBLE:{
        
      } break;
      */
      
      case ISVC:{
        int32 proc = vm.code[ vm.r[0xF]++ ];
        
        switch(proc){
          case PROC_PRINT:{
            printf("%c", vm.r[0x0]);
          } break;
          
          case PROC_EXIT:{
            running = 0x0;
          } break;
        }
      } break;
    }
    
  }
  
  return(0);
}
