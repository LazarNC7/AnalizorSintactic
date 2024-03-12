#pragma once
#include "parser.h"
#include "structDef.h"
#include "varDef.h"
#include <stdbool.h>

bool fnDef();
bool fnParam();
bool stmCompound();
bool stm();
bool expr();
bool exprAssign();
bool exprUnary();
bool exprOr();
bool exprAnd();
bool exprEq();
bool exprRel();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprPostfix();
bool exprPrimary();
  
