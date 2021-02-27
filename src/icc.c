// Main file, currently holds temporary code that uses the latest system that was implemented
// Currently used to test IR code generation, implicit casts, constant values and to check whether they create leaks

#include "parser.h"
#include "tokenstream.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char progStr[] = "b = a == b";

#define size(arr) sizeof(arr)/sizeof(arr[0])

int i = 0;
int getNextChar() { if(i < size(progStr) - 1) { return progStr[i++]; } return EOF; }
void pushLastChar(int c) { progStr[--i] = c; }

const char *actionStrings[] =
{
	#define ACTION(NAME, STR) STR,
	#include "iraction.def"
};

const char *typeStrings[] = {
	#define STRUCT_TYPE(NAME, STR) STR,
	#include "type.def"
};

const char *basicTypeStrings[] = {
	#define BASIC_TYPE(NAME, STR) STR,
	#include "type.def"
};

void stringifyType(char *s, Type *t) {
	char *tStr;
	switch(t->type) {
	case typeBasic:
		strcpy(s, basicTypeStrings[t->basic]);
		break;
	case typeRecord:
		sprintf(s, typeStrings[t->type], "struct");
		break;
	case typePointer:;
		tStr = malloc(sizeof(char) * 4096);
		stringifyType(tStr, t->pointer.type);
		sprintf(s, typeStrings[t->type], tStr);
		free(tStr);
		break;
	case typeArray:;
		tStr = malloc(sizeof(char) * 4096);
		stringifyType(tStr, t->array.type);
		sprintf(s, typeStrings[t->type], tStr, t->array.length);
		free(tStr);
		break;
	}
}

void stringifyInstr(char *s, IrInstr *instr, int levels);
void stringifyArg(char *s, IrArg *arg, int levels) {
	char *tStr;
	switch(arg->type) {
	case argSymbol:
		if(arg->s->type != symbolVariable) { /* error */ }

		tStr = malloc(sizeof(char) * 4096);
		stringifyType(tStr, arg->s->variable.type);
		sprintf(s, "[%s]", tStr);
		free(tStr);
		break;
	case argInstr:;
		if(levels) {
			strcpy(s, "t");
		} else {
			tStr = malloc(sizeof(char) * 4096);
			stringifyInstr(tStr, arg->i, levels-1);
			sprintf(s, "{ %s }", tStr);
			free(tStr);
		}
		break;
	case argConst:
		sprintf(s, "%d", arg->c->i);
	default: /* error */;
	}
}

void stringifyInstr(char *s, IrInstr *instr, int levels) {
	char *aStr = malloc(sizeof(char) * 4096);
	switch(instr->action) {
	case actionCast:
		strcpy(aStr, basicTypeStrings[instr->type]);
		break;
	case actionGoto:
	case actionCondGoto:
		stringifyInstr(aStr, &(*instr->label)->val, 0);
		break;
	default:
		stringifyArg(aStr, &instr->a, levels-1);
		break;
	}

	char *bStr = malloc(sizeof(char) * 4096);

	if(instr->action < actionSeparator) {
		// Binary action
		stringifyArg(bStr, &instr->b, levels-1);
	} else {
		strcpy(bStr, "");
	}

	sprintf(s, actionStrings[instr->action], aStr, bStr);
	free(aStr);
	free(bStr);
}

void printProg(IrProg *prog) {
	if(!prog) { return; }

	char *s = malloc(sizeof(char) * 4096);
	stringifyInstr(s, &prog->val, 1);
	puts(s);
	free(s);

	printProg(prog->next);
}

#define FAST_STR(VARNAME, STR) char *VARNAME = malloc(sizeof(char) * 4096); strcpy(VARNAME, STR);

int main() {
	Stream s = { getNextChar, pushLastChar };
	TokenStream ts = { s };
	makeTokenStream(&ts);

	ScopeContext sc;

	FunctionContext fc;
	makeFunctionContext(&fc);
	sc.fc = &fc;

	IrProg *prog = 0;
	sc.prog = &prog;

	SymbolTable st;
	makeSymbolTable(&st);

	// Make types
	Type *intT = malloc(sizeof(Type));
	intT->type = typeBasic;
	intT->basic = basicInt;


	Type *charT = malloc(sizeof(Type));
	charT->type = typeBasic;
	charT->basic = basicChar;

	// Make symbols
	Symbol *aS = malloc(sizeof(Symbol));
	aS->type = symbolVariable;
	aS->variable.type = intT;
	increaseReferencesType(intT);

	Symbol *bS = malloc(sizeof(Symbol));
	bS->type = symbolVariable;
	bS->variable.type = charT;
	increaseReferencesType(charT);

	// Insert symbols into SymbolTable
	FAST_STR(aStr, "a");
	insertEntrySymbolTable(&st, aStr, aS);

	FAST_STR(bStr, "b");
	insertEntrySymbolTable(&st, bStr, bS);

	sc.st = &st;

	IrArg arg;
	ExprRet er;
	er.arg = &arg;

	parseExpression(&er, &sc, &ts);

	printProg(prog);

	cleanIrProg(prog);
	cleanSymbolTable(&st);
	cleanFunctionContext(&fc);
	cleanType(er.type);
}
