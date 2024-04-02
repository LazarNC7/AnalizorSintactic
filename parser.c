#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "parser.h"

Token *iTk;		// the iterator in the tokens list
Token *consumedTk;		// the last consumed token

void tkerr(const char *fmt,...){
  fprintf(stderr,"error in line %d: ",iTk->line);
  va_list va;
  va_start(va,fmt);
  vfprintf(stderr,fmt,va);
  va_end(va);
  fprintf(stderr,"\n");
  exit(EXIT_FAILURE);
}

bool consume(int code){
  if(iTk->code==code){
    consumedTk=iTk;
    iTk=iTk->next;
    return true;
  }
  return false;
}

// typeBase: TYPE_INT | TYPE_DOUBLE | TYPE_CHAR | STRUCT ID
bool typeBase(){
  Token *t = iTk;
  if(consume(TYPE_INT)){
    return true;
  }
  if(consume(TYPE_DOUBLE)){
    return true;
  }
  if(consume(TYPE_CHAR)){
    return true;
  }
  if(consume(STRUCT)){
    if(consume(ID)){
      return true;
    } else tkerr("There is no ID after STRUCT in the type base");
  }
  iTk=t;
  return false;
}

// unit: ( structDef | fnDef | varDef )* END
bool unit(){
  for(;;){
      if(structDef()) {}
      else if(fnDef()) {}
      else if(varDef()) {}
      else break;
  }
  if(consume(END)){
    return true;
  }
  else tkerr("There is no END at the end of the program");
  return false;
}

//structDef: STRUCT ID LACC varDef* RACC SEMICOLON
bool structDef(){
  Token *t = iTk; 
  if(consume(STRUCT)){
    if(consume(ID)){
      if(consume(LACC)){
        while(varDef()){}
        if(consume(RACC)){
          if(consume(SEMICOLON)){
            return true;
          }
          else tkerr("There is no ; after }");
        }
        else tkerr("There is no } in struct def");
      }
      else {
        if(consume(ID)){
          if(arrayDecl()){}
          if(consume(SEMICOLON)) return true;
          else tkerr("There is no ; in struct");
        }
        else tkerr("There is no ID in struct def");
      }
    }
    else tkerr("There is no ID after STRUCT in the type def");
  }
  iTk=t; 
  return false;
}

//varDef: typeBase ID arrayDecl? SEMICOLON
bool varDef(){
  Token *t = iTk;
  if(typeBase()){
    if(consume(ID)){
      if(arrayDecl()){}
      if(consume(SEMICOLON)){
	    return true;
      }else tkerr("There is no ; in the variable definition");
    } else tkerr("There is no ID in the variable definition");
  } 
  iTk=t;
  
  return false;
}

//arrayDecl: LBRACKET INT? RBRACKET
bool arrayDecl(){
  Token *t = iTk;
  if(consume(LBRACKET)){
    if(consume(INT)) {}
    if(consume(RBRACKET)){
      return true;
    }else tkerr("There is no ] in the array declaration");
  }
  iTk=t;
  return false;
}

/*fnDef: ( typeBase | VOID ) ID
LPAR ( fnParam ( COMMA fnParam )* )? RPAR
stmCompoun
*/

bool fnDef(){
  Token *t = iTk;
  if(typeBase() || consume(VOID)){
      if(consume(ID)){
          if(consume(LPAR)){
              if(fnParam()){
                  while(1){
                      if(consume(COMMA)){
                          if(fnParam()){}
                          else{
                              iTk=t;
                              tkerr("Error at declaration of parameters' function");
                              return false;
                          }
                      }
		      else if(fnParam()){tkerr("No , in the parameters list");}
                      else break;
                  }
              }
              if(consume(RPAR)){
                  if(stmCompound()){
                      return true;
                  }
              }
              else tkerr("There is no ) in the function definition");
          }else tkerr("There is no ( in the function definition");
      }else tkerr("There is no ID in the function definition");
  }
    iTk=t;
    return false;
}


//fnParam: typeBase ID arrayDecl?
bool fnParam(){
  Token *t = iTk;
  if(typeBase()){
    if(consume(ID)){
      if(arrayDecl()){}
	  return true;
    } else tkerr("There is no ID at the parameters");
  }
  iTk=t;
  return false;
}

/*stm: stmCompound
| IF LPAR expr RPAR stm ( ELSE stm )?
| WHILE LPAR expr RPAR stm
| RETURN expr? SEMICOLON
| expr? SEMICOLON
*/

bool stm(){
    Token *t = iTk;
    if(stmCompound()) {return true;}
    if(consume(IF)){
          if(consume(LPAR)){
              if(expr()){
	
                  if(consume(RPAR)){
                      if(stm()){
			
                          if(consume(ELSE)){
                              if(stm()){
                                  return true;
                              }
                              else{
                                  tkerr("There is no statement after ELSE");
                                  iTk=t;
                                  return false;
                              }					     
                          }
                          return true;
                      }
                  } else tkerr("There is no ( in the statement");
              } else tkerr("Invalid expression in statement");
          }else tkerr("There is no ( in the statement");
    }
    if(consume(WHILE)){
          if(consume(LPAR)){
              if(expr()){
		
                  if(consume(RPAR)){
                      if(stm()){
			
                          return true;
                      }
                  } else tkerr("There is no ) after while");
              } else tkerr("Invalid expression");
          } else tkerr("There is no ( after while");
    }
    if(consume(RETURN)){
          if(expr()){}
          if(consume(SEMICOLON)){
              return true;
          } else tkerr("There is no ; after return");
    }
    if(expr()) {}
    if(consume(SEMICOLON)){
        return true;
    }
  iTk=t;
  return false;
}
  
//stmCompound: LACC ( varDef | stm )* RACC

bool stmCompound(){
    Token *t = iTk;
  if(consume(LACC)){

      for(;;){
	    if(varDef() || stm()){}
        else break;
      }
    if(consume(RACC)){
      return true;
    } else tkerr("There is no ) at the end of statement compound");
  }
  iTk=t;
  return false;
}

//expr: exprAssign

bool expr(){
    Token *t = iTk;
    if(exprAssign()) return true;
    else{
        iTk=t;
        return false;
    }
}

bool exprAssign(){
  Token *t = iTk;
  if(exprUnary()){
     if(consume(ASSIGN)){
        if(exprAssign())
            return true;
            else tkerr("There is no expression at assign");
     }
  }
  iTk=t;
  if(exprOr()) return true;
  iTk=t;
  return false;
}

bool exprOrPrim(){
    Token *t = iTk;
  if(consume(OR)){
    if(exprAnd()){
      if(exprOrPrim())
	    return true;
    } tkerr("There is no ID after OR");
  }
  iTk=t;
  return true;
}

bool exprOr(){
    Token *t = iTk;
  if(exprAnd()){
    if(exprOrPrim()) return true;

  }
  iTk=t;

  return false;
}

bool exprAndPrim(){
    Token *t = iTk;
  if(consume(AND)){
    if(exprEq()){
      if(exprAndPrim())
	    return true;
    } tkerr("There is no ID after And");
  }
  iTk=t;
  return true;
}

bool exprAnd(){
    Token *t = iTk;
  if(exprEq()){
    if(exprAndPrim()) return true;
  }
  iTk=t;
  //printf("exprand\n");
  return false;
}

bool exprEqPrim(){
    Token *t = iTk;
  if(consume(EQUAL) || consume(NOTEQ)){
    if(exprRel()){
      if(exprEqPrim())
	    return true;
    } tkerr("There is no ID after comparator");
  }
  iTk=t;
  return true;
}

bool exprEq(){
    Token *t = iTk;
  if(exprRel()){
    if(exprEqPrim()) return true;
  }
  iTk=t;
  return false;
}

bool exprRelPrim(){
    Token *t = iTk;
  if(consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)){
    if(exprAdd()){
      if(exprRelPrim())
	return true;
    } else tkerr("There is no right operand");
  }
  iTk=t;
  return true;
}

bool exprRel(){
    Token *t = iTk;
  if(exprAdd()){
    if(exprRelPrim()) return true;
  }
  iTk=t;
  return false;
}

bool exprAddPrim(){
    Token *t = iTk;
  if(consume(ADD) || consume(SUB)){
    if(exprMul()){
      if(exprAddPrim())
	    return true;
    } tkerr("There is no ID after operand (ADD fct)");
  }
  iTk=t;
  return true;
}

bool exprAdd(){
    Token *t = iTk;
  if(exprMul()){
    if(exprAddPrim()) return true;
  }
  iTk=t;
  return false;
}

bool exprMulPrim(){
    Token *t = iTk;
  if(consume(MUL) || consume(DIV)){
    if(exprCast()){
      if(exprMulPrim())
	return true;
    } else tkerr("There is no ID after operand (MUL fct)");
  }
  iTk=t;
  return true;
}

bool exprMul(){
    Token *t = iTk;
  if(exprCast()){
    if(exprMulPrim()) return true;
  }
  iTk=t;
  return false;
  
}

bool exprCast(){
    Token *t = iTk;
  if(consume(LPAR)){
    if(typeBase()){
      if(arrayDecl()){}
      if(consume(RPAR)){
	    if(exprCast()) return true;
      }else tkerr("There is no ) in the cast");
    }
  }
  else if(exprUnary()) return true;
  iTk=t;
  return false;
}

bool exprUnary(){
    Token *t = iTk;
  if(consume(SUB) || consume(NOT)){
    if(exprUnary()){
      return true;
    }else tkerr("There is no ID before sub or not");
  }
  else if (exprPostfix()) return true;
  iTk=t;
  return false;
}

bool exprPostfixPrim(){
    Token *t = iTk;
  if(consume(LBRACKET)){
    if(expr()){
      if(consume(RBRACKET)){
	    if(exprPostfixPrim()) return true;
      }else tkerr("No ] in expression postfix");
    }else tkerr("No expression in expression postfix");
  }
  else if(consume(DOT)){
            if(consume(ID)){
                if(exprPostfixPrim()) return true;
    }else tkerr("No ID in expression postfix");
  }
  iTk=t;
  return true;
}

bool exprPostfix(){
    Token *t = iTk;
  if(exprPrimary()){
    if(exprPostfixPrim()) return true;
  }
  iTk=t;
  
  return false;
}


bool exprPrimary() {
    Token *t = iTk;
  if(consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(STRING))
  return true;
  if(consume(LPAR)){
      if(expr()){
          if(consume(RPAR)) return true;
      }
  }
  if(consume(ID)){
      if(consume(LPAR)){
          if(expr()){
              for(;;){
                  if(consume(COMMA)){
                      if(expr()){} else {
                          tkerr("Invalid argument after ,");
                         }
                      
                  }
                  else break;
              }
          }
          if(consume(RPAR))
          {return true;}
      } 
    return true;
  }
  iTk=t;
  return false;
}

void parse(Token *tokens){
  iTk=tokens;
  if(!unit())tkerr("syntax error");
}
