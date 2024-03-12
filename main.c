#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "lexer.h"
#include "parser.h"

int main(){
  char* inbuf=loadFile("tests/testparser.c");
  Token* token=tokenize(inbuf);
  showTokens(token);
  unit();
  puts(inbuf);
  free(inbuf);

  return 0;
}
