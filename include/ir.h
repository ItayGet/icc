// Intermediate representation of code

#pragma once

#include "token.h"
#include "symbol.h"
#include "type.h"

typedef enum {
	#define ACTION(NAME, STR) action ## NAME,
	#include "iraction.def"
} InstrAction;

typedef enum {
	argError,
	argConst,
	argInstr,
	argSymbol,
	argBackpatch,
} ArgType;

struct IrInstr;
struct IrProg;
struct BackpatchListNode;
typedef struct {
	ArgType type;
	union {
		// A pointer to a constant
		Constant *c;

		// A pointer to the result of a diffent instruction
		struct IrInstr *i;

		// A pointer to the symbol table
		Symbol *s;

		// When an expression holds a boolean result, it will hold true
		// and false lists of instructions that need to be overwritten
		struct {
			struct BackpatchListNode *trueList;
			struct BackpatchListNode *falseList;
		} backpatch;
	};
} IrArg;

// A single instruction for an IR program
typedef struct IrInstr {
	union {
		// Regular instructions
		IrArg a;

		// Cast instructions
		BasicType type;

		// For jump instructions
		// Should point to next in IrProg
		struct IrProg **label;
	};

	IrArg b;

	InstrAction action;
} IrInstr;

// Multiple instructions
typedef struct IrProg {
	IrInstr val;
	struct IrProg *next;
} IrProg;

typedef struct BackpatchListNode {
	IrProg **val;
	struct BackpatchListNode *next;
} BackpatchListNode;

// FIXME: Might be candidate for removal?
void cleanIrProg(IrProg *prog);

// Go through a BackpatchListNode, clean nodes and replace labels inside the
// referenced instructions to parameters
void backpatchBackpatchList(BackpatchListNode *bln, IrProg **label);
