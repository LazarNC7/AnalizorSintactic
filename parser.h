#pragma once
#include<stdbool.h>
#include "lexer.h"

void parse(Token *tokens);
bool unit();
bool structDef();
bool varDef();
bool typeBase();
bool arrayDecl();
bool fnDef();
bool fnParam();
bool stm();
bool stmCompound();
bool expr();
bool exprAssign();
bool exprOr();
bool exprAnd();
bool expreEq();
bool exprRel();
bool exprEq();
bool exprAdd();
bool exprMul();
bool exprCast();
bool exprUnary();
bool exorPostfix();
bool exprPrimary();
bool exprPostfix();
