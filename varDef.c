#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "structDef.h"
#include "fnDef.h"
#include "varDef.h"

bool varDef(){
  if(typeBase()){
    if(consume(ID)){
      if(arrayDecl()){}
      if(consume(SEMICOLON)) return true;
    }
  }
  return false;
}
