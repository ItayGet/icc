// Intermediate representation of code

#pragma once

#include "token.h"
#include "symbol.h"

typedef enum {
	#define ACTION(NAME, STR) action ## NAME,
	#include "iraction-definitions.h"
} InstrAction;

typedef enum {
	argConst,
	argInstr,
	argSymbol
} ArgType;

struct IrInstr;
typedef struct {
	ArgType type;
	union {
		// A token that is a constant
		Token *t;

		// A pointer to the value of a diffent instruction
		struct IrInstr *i;

		// A pointer to the symbol table
		Symbol *s;
	};
} IrArg;

// A single instruction for an IR program
typedef struct IrInstr {
	IrArg a, b;
	InstrAction action;
} IrInstr;

// Multiple instructions
typedef struct IrProg {
	IrInstr val;
	struct IrProg *next;
} IrProg;
