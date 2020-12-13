// Parser

#pragma once

#include "ir.h"
#include "lex.h"
#include "token-stream.h"

void createAssignInstr(IrInstr *assignInstr, IrInstr *tempInstr);

void parseExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parseAssignmentExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parseAdditiveExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parsePrimaryExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
