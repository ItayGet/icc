#include "parser.h"

#include "parser-type.h"

#include <stdlib.h>
#include <stdbool.h>

void makeFunctionContext(FunctionContext *fc) {
	fc->lastConstant = &fc->constants;
	fc->constants = 0;
}

void cleanFunctionContext(FunctionContext *fc) {
	ConstantListNode *n = fc->constants;
	while(n) {
		ConstantListNode *tmp = n;
		n = n->next;

		free(tmp);
	}
}

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

	if(er->type->type == typeRecord) {
		// TODO: Assignment in records
	}

	createAssignInstr(&assignProg->val, er);

	Type *lhsType = er->type;

	parseAssignmentExpression(er, sc, ts);
	
	// Type check
	castAssignmentExpression(er, sc, lhsType);
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

		ExprRet rhsExpr, lhsExpr;

		IrProg *arithProg = malloc(sizeof(IrProg));
		IrInstr *arithInstr = &arithProg->val;
		arithInstr->action = action;
		rhsExpr.arg = &arithInstr->b;
		arithInstr->a = *er->arg;

		lhsExpr.arg = &arithInstr->a;
		lhsExpr.type = er->type;

		// TODO: Replace with correct type of expression
		parsePrimaryExpression(&rhsExpr, sc, ts);

		// Type check
		er->type = castAdditiveExpression(&lhsExpr, &rhsExpr, sc, action == actionSub);
		cleanType(lhsExpr.type);

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
		increaseReferencesType(er->type);

		free(t.identifier.name);
		break;
	case tokenIntegerConstant:;
		// FIXME: Other types of constants
		
		ConstantListNode *cln = malloc(sizeof(ConstantListNode));
		cln->c = t.constant;
		cln->next = 0;

		*sc->fc->lastConstant = cln;
		sc->fc->lastConstant = &cln->next;
		
		er->arg->type = argConst;
		er->arg->c = &cln->c;

		// OPTIMIZE: Hold an array of all basic types that will be
		// initialized first time it is read so multiple expressions
		// will have the same memory as a type
		Type *type = malloc(sizeof(Type));
		type->type = typeBasic;
		type->basic = basicInt;
		makeType(type);
		er->type = type;

		break;
	case tokenPunctuator:
		if(t.punctuator.c != puncLRBracket) { /* error */ }

		parseExpression(er, sc, ts);

		getNextToken(&t, ts);

		if(t.type != tokenPunctuator || t.punctuator.c != puncRRBracket) { /* error */ }
	default: /* error */;
	}
}
