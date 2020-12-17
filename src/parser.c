#include "parser.h"

#include <stdlib.h>
#include <stdbool.h>

void createAssignInstr(IrInstr *assignInstr, IrInstr *tempInstr) {
	switch(tempInstr->action) {
	// TODO: Check completely if lhs is l-value with type 
	case actionUAssign:
		if(tempInstr->a.type == argConst) { /* error: const cannot be l-value */ }

		assignInstr->action = actionAssign;
		assignInstr->a = tempInstr->a;
		return;
	default: /* error: not lvalue */;
	}
}

void addInstructionAsArg(IrArg *arg, IrProg *tempProg, IrProg **prog) {
	if(tempProg->val.action == actionUAssign) {
		// Get rid of unary assigns
		*arg = tempProg->val.a;
		free(tempProg);
	} else {
		arg->type = argInstr;
		arg->i = &tempProg->val;
		(*prog)->next = tempProg;
		*prog = tempProg;
		tempProg->next = NULL;
	}
}

void widenArg(IrArg *arg, TypeType curr, TypeType wide, IrProg **prog) {
	if(curr == wide) { return; }

	IrProg *cast = malloc(sizeof(IrProg));

	cast->val.action = actionCast;
	cast->val.type = wide;
	cast->val.b = *arg;

	arg->type = argInstr;
	arg->i = cast;

	(*prog)->next = cast;
	*prog = cast;

}

void parseExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	parseAssignmentExpression(retInstr, prog, ts);

	Token t;
	getNextToken(&t, ts);

	// Assignment expression
	if(t.type != tokenPunctuator || t.punctuator.c != puncComma) { pushBackToken(ts, &t); return; }

	parseExpression(retInstr, prog, ts);
}

void parseAssignmentExpression(IrInstr *retInstr, IrProg **prog, TokenStream *ts) {
	parseAdditiveExpression(retInstr, prog, ts);

	Token t;
	getNextToken(&t, ts);

	// Production without assignmentOperator
	if(t.type != tokenPunctuator || t.punctuator.c != puncEqual) { pushBackToken(ts, &t); return; }

	IrProg *rhsProg = malloc(sizeof(IrProg));
	parseAssignmentExpression(&rhsProg->val, prog, ts);

	IrProg *assignProg = malloc(sizeof(IrProg));
	createAssignInstr(&assignProg->val, retInstr);
	addInstructionAsArg(&assignProg->val.b, rhsProg, prog);

	(*prog)->next = assignProg;
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

		// Create a new return insturction
		retInstr->action = action;
		addInstructionAsArg(&retInstr->a, aProg, prog);
		addInstructionAsArg(&retInstr->b, bProg, prog);
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
