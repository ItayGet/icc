// Parser

#pragma once

#include "ir.h"
#include "lex.h"
#include "token-stream.h"
#include "type.h"

#include <stdbool.h>

// TODO: Add type checking

// General paramters for parsing functions:
// retInstr - Returns the IR instruction that contains the temporary that was calculated
// prog - A reference to a pointer of the last node of the IR program that is currently being built
// 	If it weren't a reference to the pointer, after calling a parsing function you would have to
// 	move the pointer all the way to the end every time
// ts - The TokenStream from which all tokens are popped

// Return assignInstr from an instruction that creates a temporary,
// throw error if necessary
void createAssignInstr(IrInstr *assignInstr, IrInstr *tempInstr);

// Add a argument that is a temporary to an action instruction,
// add the temporary instruction to the program if necessary
void addInstructionAsArg(IrArg *arg, IrProg *tempProg, IrProg **prog);

// Widen argument by adding a casting instruction
void widenArg(IrArg *arg, Type *curr, Type *wide, IrProg **prog);

void parseExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parseAssignmentExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parseAdditiveExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
void parsePrimaryExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts);
