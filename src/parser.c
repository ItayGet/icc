#include "parser.h"

#include <stdlib.h>
#include <stdbool.h>

void createAssignInstr(IrInstr *assignInstr, ExprRet *er) {
	switch(er->arg->type) {
	case argSymbol:
		// A variable is the only viable lvalue
		if(er->arg->s->type != symbolVariable) { /* error */ }

		assignInstr->action = actionAssign;

		assignInstr->a = *er->arg;
		break;
	case argInstr:;
		// An instruction that is already part of the program that is being built
		IrInstr *tempInstr = er->arg->i;

		switch(tempInstr->action) {
		case actionAssignDerefR:
			if(er->type->type == typeArray) { /* error */ }

			assignInstr->action = actionAssignDerefL;
			assignInstr->a = tempInstr->a;
			
			// Disable the temporary instruction
			tempInstr->action = actionNOP;
			break;
		case actionArrayAccess:
			if(er->type->type == typeArray) { /* error */ }
			
			// If the original instruction was
			// t_n = a[i]
			// Now the action will be
			// t_n = a + i
			// And assignInstr
			// *t_n = b

			assignInstr->action = actionAssignDerefL;
			assignInstr->a = *er->arg;

			tempInstr->action = actionAdd;
			break;
		default: /* eror */;
		}
	default: /* error */;
	}
}

void parseExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	parseAssignmentExpression(er, sc, ts);

	Token t;
	getNextToken(&t, ts);

	// Assignment expression
	if(t.type != tokenPunctuator || t.punctuator.c != puncComma) { pushBackToken(ts, &t); return; }

	parseExpression(er, sc, ts);
}

void parseAssignmentExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	parseAdditiveExpression(er, sc, ts);

	Token t;
	getNextToken(&t, ts);

	// Production without assignmentOperator
	if(t.type != tokenPunctuator || t.punctuator.c != puncEqual) { 
		pushBackToken(ts, &t);
		t.type = tokenPunctuator;
		return;
	}

	if(er->type->type == typeFunction) { /* error */ }
	if(er->type->type == typeArray) { /* error */ }

	IrProg *assignProg = malloc(sizeof(IrProg));
	createAssignInstr(&assignProg->val, er);

	Type *lhsType = er->type;

	parseAssignmentExpression(er, sc, ts);
	
	// Type check
	if(lhsType->type == typeRecord) {
		// Either a different type, which is an error, or a different
		// record, which still is an error
		if(lhsType != er->type) { /* error */ }

		// TODO: Record equality
	} else if(IS_BASIC_TYPE(lhsType->type)) {
		if(!IS_BASIC_TYPE(er->type->type)) { /* error */ }

		widenArg(er->arg, sc, er->type->type, lhsType->type);
	} else {
		// Only thing left is a pointer
		
		// TODO: Function assignment

		if(er->type->type != typeArray && er->type->type != typePointer) { /* error */ }
	}

	cleanType(lhsType);

	assignProg->val.b = *er->arg;

	*sc->prog = assignProg;
	sc->prog = &assignProg->next;
}

void parseAdditiveExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	parsePrimaryExpression(er, sc, ts);

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

		ExprRet rhsExpr;

		IrProg *arithProg = malloc(sizeof(IrProg));
		IrInstr *arithInstr = &arithProg->val;
		arithInstr->action = action;
		rhsExpr.arg = &arithInstr->b;
		arithInstr->a = *er->arg;

		// TODO: Replace with correct type of expression
		parsePrimaryExpression(&rhsExpr, sc, ts);

		Type *lhsType = er->type;

		TypeType lhsBType = er->type->type;
		TypeType rhsBType = rhsExpr.type->type;

		// Type check
		if(IS_BASIC_TYPE(lhsBType) && IS_BASIC_TYPE(rhsBType)) {
			Type *wide = lhsBType > rhsBType ? lhsType : rhsExpr.type;
			widenArg(&arithInstr->a, sc, lhsBType, wide->type);
			widenArg(&arithInstr->b, sc, rhsBType, wide->type);

			er->type = wide;
		} else if(lhsBType == typePointer || rhsBType == typePointer) {
			// TODO: Pointer arithmetic
			// Before that a better representation for the token inside IrArg should be implemented
		} else { /* error */ }

		increaseReferences(er->type);
		cleanType(lhsType);
		cleanType(rhsExpr.type);

		// Set new value for er
		er->arg->type = argInstr;
		er->arg->i = &arithProg->val;

		*sc->prog = arithProg;
		sc->prog = &arithProg->next;
	}
	breakLoop:
	pushBackToken(ts, &t);
}

void parsePrimaryExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	switch(t.type) {
	case tokenIdentifier:;
		Symbol *s = getValueSymbolTable(sc->st, t.identifier.name);

		if(s == NULL) { /* error */ }
		if(s->type != symbolVariable) { /* error */ }

		er->arg->type = argSymbol;
		er->arg->s = s;

		er->type = s->variable.type;
		increaseReferences(er->type);

		free(t.identifier.name);
		break;
	case tokenIntegerConstant:
		// TODO: Parse integer constants
		break;
	case tokenPunctuator:
		if(t.punctuator.c != puncLRBracket) { /* error */ }

		parseExpression(er, sc, ts);

		getNextToken(&t, ts);

		if(t.type != tokenPunctuator || t.punctuator.c != puncRRBracket) { /* error */ }
	default: /* error */;
	}
}
