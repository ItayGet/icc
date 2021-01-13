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
} ArgType;

struct IrInstr;
typedef struct {
	ArgType type;
	union {
		// A pointer to a constant
		Constant *c;

		// A pointer to the result of a diffent instruction
		struct IrInstr *i;

		// A pointer to the symbol table
		Symbol *s;
	};
} IrArg;

// A single instruction for an IR program
typedef struct IrInstr {
	union {
		// Regular instructions
		IrArg a;

		// Cast instructions
		BasicType type;

		// Goto instructions
		// TODO: Label label;
	};

	IrArg b;

	InstrAction action;
} IrInstr;

// Multiple instructions
typedef struct IrProg {
	IrInstr val;
	struct IrProg *next;
} IrProg;

void cleanIrProg(IrProg *prog);
