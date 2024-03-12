#pragma once
#include <stdbool.h>
#include "lexer.h"
#include "structDef.h"
#include "fnDef.h"
#include "varDef.h"

void parse(Token *tokens);
bool consume(int code);
bool typeBase();
bool unit();
