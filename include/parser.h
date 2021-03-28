// Parser

#pragma once

#include "ir.h"
#include "lex.h"
#include "tokenstream.h"
#include "type.h"
#include "symboltable.h"

#include <stdbool.h>

// Type of return expression
typedef enum {
	exprRetRegular,
	exprRegBackpatch,
} ExprRetType;

// The result of expressions
typedef struct {
	ExprRetType retType;

	union {
		// The normal result of an expression
		struct {
			// An argument that is the result of the expression
			IrArg *arg;

			// The type of the expression
			// If arg has type argBackpatch type should be cleaned
			Type *type;
		};
		
		// When an expression holds a boolean result, it will hold true
		// and false lists of instructions that need to be overwritten
		struct {
			struct BackpatchListNode *trueList;
			struct BackpatchListNode *falseList;
		} backpatch;
	};
} ExprRet;

// The context of the current scope
typedef struct {
	// A pointer to the last instruction in the context
	// FIXME: Is a double pointer really needed?
	IrProg **prog;

	// TODO: Should be able to use multiple scopes
	// A pointer to the SymbolTable of the last scope
	SymbolTable *st;

	// A pointer to the function context
	struct FunctionContext *fc;
} ScopeContext;

// A list to store all constants inside a function's IR
typedef struct ConstantListNode {
	Constant c;
	struct ConstantListNode *next;
} ConstantListNode;

typedef struct FunctionContext {
	// TODO: Store a list of all symboltables used inside function
	
	ConstantListNode *constants;
	ConstantListNode **lastConstant;
} FunctionContext;

void makeFunctionContext(FunctionContext *fc);

void cleanFunctionContext(FunctionContext *fc);

// Return assignInstr, an instruction that is the assigning equivalent of tempArg
// throw error if necessary
void createAssignInstr(IrInstr *assignInstr, ExprRet *er);

// Turn exprRegBackpatch to a temporary by making a new temporary and assigning it
// to either 1 or 0 for each true and false list respectively
void createTemporaryFromBackpatch(ExprRet *er, ScopeContext *sc);

// Turn a regular argument to a backpatch by comparing it to 0
void createBackpatchFromArg(ExprRet *er, ScopeContext *sc);

void parseExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parseAssignmentExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
//TODO: Generalize all binary outputs into one
void parseLogicalAndExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parseEqualityExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parseAdditiveExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
void parsePrimaryExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts);
