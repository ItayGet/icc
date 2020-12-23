// Parser

#pragma once

#include "ir.h"
#include "lex.h"
#include "token-stream.h"
#include "type.h"
#include "symboltable.h"

#include <stdbool.h>

// The result of expressions
typedef struct {
	// An argument that is the result of the expression
	IrArg *arg;

	// The type of the expression
	Type *type;
} ExprRet;

// The context of the current scope
typedef struct {
	// A pointer to the last instruction in the context
	IrProg **prog;

	// TODO: Should be able to use multiple scopes
	// A pointer to the SymbolTable of the last scope
	SymbolTable *st;
} ScopeContext;

// Return assignInstr, an instruction that is the assigning equivalent of tempArg
// throw error if necessary
void createAssignInstr(IrInstr *assignInstr, ExprRet *er);

// Widen argument by adding a casting instruction
void widenArg(IrArg *arg, ScopeContext *sc, TypeType curr, TypeType wide);

void parseExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parseAssignmentExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parseAdditiveExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parsePrimaryExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
