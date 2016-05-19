#include <stdio.h>
#include <string.h>

#include "vm.h"

#define LINESMAX 20
#define CHARSMAX 255
#define BUFFERSIZE LINESMAX*CHARSMAX

int main()
{
  /*
    begin:
    ldr 4, 9
    svc 16
    
    cmpi 0, 19
    beq exit
    b begin
    
    exit:
    ldr 4, 1
    svc 16
  */
  
  FILE *input;
  FILE *output;
  char *buffer;
  int i;
  
  input  = fopen("program.txt", "r");
  output = fopen("program.abc", "wb");
  
  buffer  = (char *)malloc(BUFFERSIZE*sizeof(char));
  
  while( !feof(input) ){
    char *bufptr = buffer;
    int linesread = 0;
    
    for( i = 0; i < LINESMAX; i++ ){
      if( fgets(bufptr, CHARSMAX, input) != NULL ){
        if( strlen(bufptr) > 1 ){
          bufptr = strchr(bufptr, '\n');
          *bufptr++ = '\0';
          linesread++;
        }
      } else{
        //NOTE(adm244): error, exit?
      }
    }
    
    //TODO(adm244): read buffer and write bytecode
    break;
  }
  
  fclose(output);
  fclose(input);
  
  return(0);
}
