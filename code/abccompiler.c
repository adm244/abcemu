/*
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
*/

#include <stdio.h>
#include <string.h>

#include "vm.h"

#define LINESMAX 5
#define CHARSMAX 255
#define BUFFERSIZE LINESMAX*CHARSMAX

void getoprnd(char *str, int32 *op);
void getoprnds(char *str, int32 *op1, int32 *op2);

void memwrite(int32 *buffer, size_t *size, size_t *pos, int32 opcode, const int32 *op1, const int32 *op2);

int main()
{
  FILE *input;
  FILE *output;
  
  int32 *bufres;
  size_t bufressize = BUFSIZ;
  size_t bufwritten = 0;
  
  char *buffer;
  char *strs[LINESMAX];
  
  int i;
  
  printf("Opening files...\n");
  
  input  = fopen("program.txt", "r");
  output = fopen("program.abc", "wb");
  
  buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
  bufres = (int32 *)malloc(bufressize*sizeof(int32));
  
  printf("Parsing started...\n\n");
  
  while( !feof(input) ){
    char *bufptr = buffer;
    int linesread = 0;
    
    for( i = 0; i < LINESMAX; i++ ){
      if( fgets(bufptr, CHARSMAX, input) != NULL ){
        if( strlen(bufptr) > 1 ){
          strs[linesread++] = bufptr;
          bufptr = strchr(bufptr, '\n');
          *bufptr++ = '\0';
        }
      } else{
        //NOTE(adm244): error, exit?
      }
    }
    
    for( i = 0; i < linesread; i++ ){
      char *start;
      char *end;
      int32 op1 = 0;
      int32 op2 = 0;
      
      start = strs[i];
      end = strchr(start, 0x20);
      if( end == NULL ){
        end = strchr(start, ':');
        if( end != NULL ){
          *end = 0x0;
          printf("%s:\n", start);
          
          //TODO(adm244): labels!!
          
          continue;
        }
      }
      *end++ = 0x0;
      
      #define INSTOP2(MNEMONIC, OPCODE, OP1, OP2)\
      if( strcmp(start, MNEMONIC) == 0 ){\
        getoprnds(end, OP1, OP2);\
        memwrite(bufres, &bufressize, &bufwritten, OPCODE, OP1, OP2);\
        printf("%s 0x%X, 0x%X\n", MNEMONIC, *OP1, *OP2);\
      }
      
      INSTOP2("ldr", ILDR, &op1, &op2)
      INSTOP2("ldrm", ILDRM, &op1, &op2)
      INSTOP2("ldmr", ILDMR, &op1, &op2)
      INSTOP2("str", ISTR, &op1, &op2)
      INSTOP2("mov", IMOV, &op1, &op2)
      INSTOP2("add", IADD, &op1, &op2)
      INSTOP2("adds", IADDS, &op1, &op2)
      INSTOP2("sub", ISUB, &op1, &op2)
      INSTOP2("subs", ISUBS, &op1, &op2)
      INSTOP2("mul", IMUL, &op1, &op2)
      INSTOP2("muls", IMULS, &op1, &op2)
      INSTOP2("div", IDIV, &op1, &op2)
      INSTOP2("divs", IDIVS, &op1, &op2)
      INSTOP2("ands", IANDS, &op1, &op2)
      INSTOP2("eors", IEORS, &op1, &op2)
      INSTOP2("orrs", IORRS, &op1, &op2)
      INSTOP2("bics", IBICS, &op1, &op2)
      INSTOP2("tst", ITST, &op1, &op2)
      INSTOP2("lsls", ILSLS, &op1, &op2)
      INSTOP2("lsrs", ILSRS, &op1, &op2)
      INSTOP2("cmp", ICMP, &op1, &op2)
      INSTOP2("cmpi", ICMPI, &op1, &op2)
      INSTOP2("cmn", ICMN, &op1, &op2)
      INSTOP2("mvns", IMVNS, &op1, &op2)
      #undef INSTOP2
      
      #define INSTOP1(MNEMONIC, OPCODE, OP1)\
      if( strcmp(start, MNEMONIC) == 0 ){\
        getoprnds(end, OP1, NULL);\
        memwrite(bufres, &bufressize, &bufwritten, OPCODE, OP1, NULL);\
        printf("%s 0x%X\n", MNEMONIC, *OP1);\
      }
      
      INSTOP1("push", IPUSH, &op1)
      INSTOP1("pop", IPOP, &op1)
      INSTOP1("svc", ISVC, &op1)
      INSTOP1("b", IB, &op1)
      INSTOP1("beq", IBEQ, &op1)
      INSTOP1("bne", IBNE, &op1)
      INSTOP1("bcs", IBCS, &op1)
      INSTOP1("bcc", IBCC, &op1)
      INSTOP1("bmi", IBMI, &op1)
      INSTOP1("bpl", IBPL, &op1)
      INSTOP1("bvs", IBVS, &op1)
      INSTOP1("bvc", IBVC, &op1)
      INSTOP1("bge", IBGE, &op1)
      INSTOP1("blt", IBLT, &op1)
      INSTOP1("bgt", IBGT, &op1)
      INSTOP1("ble", IBLE, &op1)
      INSTOP1("negs", INEGS, &op1)
      #undef INSTOP1
    }
  }
  
  printf("\nWriting bytecode...\n");
  
  fwrite(bufres, sizeof(int32), bufwritten, output);
  
  fclose(output);
  fclose(input);
  
  printf("Done!\n");
  
  return(0);
}

void getoprnds(char *str, int32 *op1, int32 *op2)
//TODO(adm244): check for errors
{
  char **temp = NULL;
  
  *op1 = (int32)strtol(str, temp, 0);
  
  if( op2 != NULL ){
    str = strchr(str, ',') + 1;
    *op2 = (int32)strtol(str, temp, 0);
  }
}

void memwrite(int32 *buffer, size_t *size, size_t *pos, int32 opcode, const int32 *op1, const int32 *op2)
{
  if( *pos > *size - 3 ){
    *size += BUFSIZ;
    buffer = (int32 *)realloc(buffer, *size*sizeof(int32));
    //TODO(adm244): check if memory was allocated successefully
  }
  
  buffer[*pos] = opcode;
  *pos += 1;
  
  if( op1 != NULL ){
    buffer[*pos] = *op1;
    *pos += 1;
    
    if( op2 != NULL ){
      buffer[*pos] = *op2;
      *pos += 1;
    }
  }
}
