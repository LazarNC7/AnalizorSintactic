#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "structDef.h"
#include "fnDef.h"
#include "varDef.h"

bool exprPrimary(){
  if(consume(ID)){
    if(consume(LPAR)){
      if(expr()){
	while(consume(COMMA)&&expr()){}
      }
      if(consume(RPAR)){}
      
    }
    
    return true;
  }

  if(consume(INT))return true;
  if(consume(DOUBLE))return true;
  if(consume(CHAR))return true;
  if(consume(STRING))return true;
  if(consume(LPAR)){
    if(expr()){
      if(consume(RPAR))return true;
    }
  }

  return false;
}

bool exprPostfix(){
  if(exprPostfix()){
    if(consume(LBRACKET)){
      if(expr()){
	if(consume(RBRACKET))return true;
      }
    }
  }

  if(exprPostfix()){
    if(consume(DOT)){
      if(consume(ID))return true;
    }
  }

  if(exprPrimary())return true;
  return false;
}

bool exprUnary(){
  if(exprPostfix())return true;
  if(consume(SUB)||consume(NOT)){
    if(exprUnary()) return true;
  }

  return false;
}

bool exprCast(){
  if(exprUnary())return true;
  if(consume(LPAR)){
    if(typeBase()){
      if(arrayDecl()){}
      if(consume(RPAR)){
	if(exprCast())return true;
      }
    }
  }
  return false;
}

bool exprMul(){
  if(exprCast())return true;
  if(exprMul()){
    if(consume(MUL)||consume(DIV)){
      if(exprCast())return true;
    }
  }
  return false;
}

bool exprAdd(){
  if(exprMul())return true;
  if(exprAdd()){
    if(consume(ADD)||consume(SUB)){
      if(exprMul()) return true;
    }
  }

  return false;
}

bool exprRel(){
  if(exprAdd())return true;
  if(exprRel()){
    if(consume(LESS)||consume(LESSEQ)||consume(GREATER)||consume(GREATEREQ)){
      if(exprAdd())return true;
    }
  }
  return false;
}

bool exprEq(){
  if(exprRel())return true;
  if(exprEq()){
    if(consume(EQUAL)||consume(NOTEQ)){
      if(exprRel())return true;
    }
  }
  return false;
}

bool exprAnd(){
  if(exprEq())return true;
  if(exprAnd()){
    if(consume(AND)){
      if(exprEq()) return true;
    }
  }
  return false;
}

bool exprOr(){
  if(exprAnd())return true;
  if(exprOr()){
    if(consume(OR)){
      if(exprAnd()){
	return true;
      }
    }
  }
  return false;
}

bool exprAssign(){
  if(exprOr())return true;
  if(exprUnary()){
    if(consume(ASSIGN)){
      if(exprAssign()){
	return true;
      }
    }

  }

  return false;
}

bool expr(){
  if(exprAssign())return true;
  return false;
}

bool stmCompound(){
  if(consume(LACC)){
    while(varDef()||stm());
    if(consume(RACC))return true;
  }

  return false;
}

bool stm(){
  if(stmCompound())return true;
  if(consume(IF)){
    if(consume(LPAR)){
      if(expr()){
	if(consume(RPAR)){
	  if(stm()){
	    if(consume(ELSE)&&stm()){}
	    return true;
	  }
	}
      }
    }
  }

  if(consume(WHILE)){
    if(consume(LPAR)){
      if(expr()){
	if(consume(RPAR)){
	  if(stm()) return true;
	}
      }
    }
  }

  if(consume(RETURN)){
    if(expr()){}
    if(consume(SEMICOLON)) return true;
  }

  if(expr()){}
  if(consume(SEMICOLON)) return true;

  return false;
}

bool fnParam(){
  if(typeBase()){
    if(consume(ID)){
      if(arrayDecl()){}
      return true;
    }
  }

  return false;
}

bool fnDef(){
  if(typeBase()||consume(VOID)){
    if(consume(ID)){
      if(consume(LPAR)){
	if(fnParam()){
	  while(consume(COMMA)&&fnParam());
	}
	if(consume(RPAR)){
	  if(stmCompound()) return true;
	}
      }
    }
  }

  return false;
}
