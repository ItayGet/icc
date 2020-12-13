#include "parser.h"

#include <stdlib.h>
#include <stdbool.h>

// TODO: Add type checking

// General paramters for parsing functions:
// retInstr - Returns the IR instruction that contains the temporary that was calculated
// prog - A reference to  a pointer of the last node of the IR program that is currently being built
// 	If it weren't a reference to the pointer, after calling a parsing function you would have to
// 	move the pointer all the way to the end every time
// ts - The TokenStream from which all tokens are popped

// Return assignInstr from an instruction that creates a temporary,
// throw error if necessary
void createAssignInstr(IrInstr *assignInstr, IrInstr *tempInstr) {
	switch(tempInstr->action) {
	// TODO: Check completely if lhs is l-value with type 
	case actionUAssign:
		if(tempInstr->a.type == argConst) { /* error: const cannot be l-value */ }

		assignInstr->action = actionAssign;
		assignInstr->a = tempInstr->a;
		return;
	default: /* error */;
	}
}

void parseExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	parseAssignmentExpression(retInstr, prog, ts);

	Token t;
	getNextToken(&t, ts);

	// Assignment expression
	if(t.type != puncComma) { pushBackToken(ts, &t); return; }

	parseExpression(retInstr, prog, ts);
}

void parseAssignmentExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	parseAdditiveExpression(retInstr, prog, ts);

	Token t;
	getNextToken(&t, ts);

	// Production without assignmentOperator
	if(t.type != tokenPunctuator || t.punctuator.c != puncEqual) { pushBackToken(ts, &t); return; }

	// Prepare for inserting final right hand side expression before assignment instruction
	IrProg *rhsProg = malloc(sizeof(IrProg));
	parseAssignmentExpression(&rhsProg->val, prog, ts);

	IrProg *assignProg = malloc(sizeof(IrProg));
	createAssignInstr(&assignProg->val, retInstr);
	assignProg->val.b.type = argInstr;
	assignProg->val.b.i = &rhsProg->val;

	(*prog)->next = rhsProg;
	rhsProg->next = assignProg;

	assignProg->next = NULL;

	(*prog) = assignProg;
}


void parseAdditiveExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	parsePrimaryExpression(retInstr, prog, ts);

	Token t;
	while(true) {
		getNextToken(&t, ts);

		if(t.type != tokenPunctuator) { break; }

		InstrAction action;
		switch(t.punctuator.c) {
		case puncPlus:
			action = actionAdd;
			break;
		case puncMinus:
			action = actionSub;
			break;
		default: 
			goto breakLoop;
		}

		IrProg *aProg = malloc(sizeof(IrProg));
		aProg->val = *retInstr;

		IrProg *bProg = malloc(sizeof(IrProg));
		parsePrimaryExpression(&bProg->val, prog, ts);

		retInstr->action = action;

		retInstr->a.type = argInstr;
		retInstr->a.i = &aProg->val;

		retInstr->b.type = argInstr;
		retInstr->b.i = &bProg->val;

		(*prog)->next = aProg;
		aProg->next = bProg;

		bProg->next = NULL;

		(*prog) = bProg;

	}
	breakLoop:
	pushBackToken(ts, &t);
}

void parsePrimaryExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	// FIXME: t not freed
	Token *t = malloc(sizeof(Token));
	getNextToken(t, ts);

	Symbol *s = malloc(sizeof(Symbol));
	switch(t->type) {
	case tokenIdentifier:
		// TODO: Get from symbol table

		// FIXME: s not freed
		s->type = symbolVariable;
		s->variable.name = t->identifier.name;

		retInstr->action = actionUAssign;
		retInstr->a.type = argSymbol;
		retInstr->a.s = s;
		break;
	case tokenIntegerConstant:
		retInstr->action = actionUAssign;
		retInstr->a.type = argConst;
		retInstr->a.t = t;
		break;
	case tokenPunctuator:
		if(t->punctuator.c != puncLRBracket) { /* error */ }

		parseExpression(retInstr, prog, ts);

		getNextToken(t, ts);

		if(t->type != tokenPunctuator || t->punctuator.c != puncRRBracket) { /* error */ }
	default: /* error */;
	}
}
