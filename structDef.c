#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "structDef.h"
#include "fnDef.h"
#include "varDef.h"

bool arrayDecl(){
  if(consume(LBRACKET)){
    if(consume(INT)){}
    if(consume(RBRACKET)){
      return true;
    }
  }

  return false;
}



bool structDef(){
  if(consume(STRUCT)){
    if(consume(ID)){
      if(consume(LACC)){
	while(varDef()){}
	if(consume(RACC)){
	  if(consume(SEMICOLON)){
	    return true;
	  }
	}
      }
    }
  }

  return false;

}
