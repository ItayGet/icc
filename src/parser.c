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
			tempInstr->action = actionNop;
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

void createTemporaryFromBackpatch(ExprRet *er, ScopeContext *sc) {
	if(er->retType != exprRegBackpatch) { return; }

	// OPTIMIZE: Use global types
	// HACK: Determine the type for the result of comparisons, for now use int
	Type *exprType = malloc(sizeof(Type));
	makeType(exprType);
	exprType->type = typeBasic;
	exprType->basic = basicInt;

	Symbol *temp = malloc(sizeof(Symbol));
	temp->type = symbolVariable;
	temp->variable.type = exprType;
	increaseReferencesType(exprType);

	// A temporary so key is empty string
	char *key = malloc(sizeof(char) * 1);
	*key = 0;

	insertEntrySymbolTable(sc->st, key, temp);

	// OPTIMIZE: Have global true and false constants?
	ConstantListNode *zeroCln = malloc(sizeof(ConstantListNode));
	ConstantListNode *oneCln = malloc(sizeof(ConstantListNode));
	zeroCln->c.i = 0;
	oneCln->c.i = 1;
	*sc->fc->lastConstant = zeroCln;
	zeroCln->next = oneCln;
	sc->fc->lastConstant = &oneCln->next;

	// in IR: temp = 1
	IrProg *trueProg = malloc(sizeof(IrProg));
	trueProg->val.action = actionAssign;
	trueProg->val.a.type = argSymbol;
	trueProg->val.a.s = temp;
	trueProg->val.b.type = argConst;
	trueProg->val.b.c = &oneCln->c;

	// Instruction to skip assignment of temp to 0
	// in IR: goto <uninitialized>
	IrProg *gotoProg = malloc(sizeof(IrProg));
	gotoProg->val.action = actionGoto;

	trueProg->next = gotoProg;

	// in IR: temp = 0
	IrProg *falseProg = malloc(sizeof(IrProg));
	falseProg->val.action = actionAssign;
	falseProg->val.a.type = argSymbol;
	falseProg->val.a.s = temp;
	falseProg->val.b.type = argConst;
	falseProg->val.b.c = &zeroCln->c;

	gotoProg->next = falseProg;

	gotoProg->val.label = &falseProg->next;

	// Add created instructions into the program
	*sc->prog = trueProg;

	backpatchBackpatchList(er->backpatch.trueList, sc->prog);
	backpatchBackpatchList(er->backpatch.falseList, &gotoProg->next);

	sc->prog = &falseProg->next;

	er->arg->type = argSymbol;
	er->arg->s = temp;

	er->type = exprType;
	er->retType = exprRetRegular;
}

void createBackpatchFromArg(ExprRet *er, ScopeContext *sc) {
	if(er->retType != exprRetRegular) { return; }

	cleanType(er->type);

	// OPTIMIZE: Have global true and false constants?
	ConstantListNode *zeroCln = malloc(sizeof(ConstantListNode));
	zeroCln->c.i = 0;
	zeroCln->next = NULL;
	*sc->fc->lastConstant = zeroCln;
	sc->fc->lastConstant = &zeroCln->next;

	IrProg *compProg = malloc(sizeof(IrProg));
	compProg->val.a = *er->arg;
	compProg->val.b.type = argConst;
	compProg->val.b.c = &zeroCln->c;
	compProg->val.action = actionCompEqual;

	IrProg *condGotoProg = malloc(sizeof(IrProg));
	condGotoProg->val.action = actionCondGoto;

	IrProg *gotoProg = malloc(sizeof(IrProg));
	gotoProg->val.action = actionGoto;

	// OPTIMIZE: Make this a function
	// Create backpackpatch lists
	BackpatchListNode *trueList = malloc(sizeof(BackpatchListNode));
	trueList->val = &compProg->next;
	trueList->next = NULL;

	BackpatchListNode *falseList = malloc(sizeof(BackpatchListNode));
	falseList->val = &condGotoProg->next;
	falseList->next = NULL;

	er->retType = exprRegBackpatch;
	er->backpatch.trueList = trueList;
	er->backpatch.falseList = falseList;

	compProg->next = condGotoProg;
	condGotoProg->next = gotoProg;

	*sc->prog = compProg;
	sc->prog = &gotoProg->next;
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
	parseLogicalAndExpression(er, sc, ts);

	Token t;
	getNextToken(&t, ts);

	// Production without assignmentOperator
	if(t.type != tokenPunctuator || t.punctuator.c != puncEqual) { 
		pushBackToken(ts, &t);
		return;
	}

	createTemporaryFromBackpatch(er, sc);

	if(er->type->type == typeFunction) { /* error */ }
	if(er->type->type == typeArray) { /* error */ }

	IrProg *assignProg = malloc(sizeof(IrProg));

	if(er->type->type == typeRecord) {
		// TODO: Assignment in records
	}

	createAssignInstr(&assignProg->val, er);

	Type *lhsType = er->type;

	parseAssignmentExpression(er, sc, ts);
	createTemporaryFromBackpatch(er, sc);
	
	// Type check
	castAssignmentExpression(er, sc, lhsType);
	cleanType(lhsType);

	er->retType = exprRetRegular;
	
	assignProg->val.b = *er->arg;

	*sc->prog = assignProg;
	sc->prog = &assignProg->next;
}

void parseLogicalAndExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	parseEqualityExpression(er, sc, ts);

	Token t;
	getNextToken(&t, ts);
	
	if(t.type != tokenPunctuator || t.punctuator.c != puncDAmp) {
		pushBackToken(ts, &t);
		return;
	}

	createBackpatchFromArg(er, sc);

	// sc->prog will later point to the calculation of the rhs of the expression
	backpatchBackpatchList(er->backpatch.trueList, sc->prog);

	// Save falseList to concatenate with rhs of expression
	BackpatchListNode *lhsFalseList = er->backpatch.falseList;

	parseEqualityExpression(er, sc, ts);
	createBackpatchFromArg(er, sc);

	// Concatenate rhs.falseList with lhs.falseList
	BackpatchListNode** lastNode = &er->backpatch.falseList;
	for(; *lastNode; lastNode = &(*lastNode)->next);
	*lastNode = lhsFalseList;
}

// FIXME: Currently a proof of concept, fix later
void parseEqualityExpression(ExprRet *er, ScopeContext *sc, TokenStream *ts) {
	parseAdditiveExpression(er, sc, ts);

	Token t;
	getNextToken(&t, ts);
	
	if(t.type != tokenPunctuator || t.punctuator.c != puncDEqual) {
		pushBackToken(ts, &t);
		return;
	}

	createTemporaryFromBackpatch(er, sc);

	IrProg *compProg = malloc(sizeof(IrProg));
	compProg->val.action = actionCompEqual;
	compProg->val.a = *er->arg;

	ExprRet rhsExpr;
	rhsExpr.arg = &compProg->val.b;

	// FIXME: Can't chain equality expressions (eg a == b == c)
	parseAdditiveExpression(&rhsExpr, sc, ts);
	createTemporaryFromBackpatch(&rhsExpr, sc);

	// TODO: Type check
	
	cleanType(er->type);
	cleanType(rhsExpr.type);

	IrProg *condGotoProg = malloc(sizeof(IrProg));
	condGotoProg->val.action = actionCondGoto;

	compProg->next = condGotoProg;

	IrProg *gotoProg = malloc(sizeof(IrProg));
	gotoProg->val.action = actionGoto;

	condGotoProg->next = gotoProg;

	// Create backpackpatch lists
	BackpatchListNode *trueList = malloc(sizeof(BackpatchListNode));
	trueList->val = &compProg->next;
	trueList->next = NULL;

	BackpatchListNode *falseList = malloc(sizeof(BackpatchListNode));
	falseList->val = &condGotoProg->next;
	falseList->next = NULL;

	er->retType = exprRegBackpatch;
	er->backpatch.trueList = trueList;
	er->backpatch.falseList = falseList;

	*sc->prog = compProg;
	sc->prog = &gotoProg->next;
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

		createTemporaryFromBackpatch(er, sc);

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
		createTemporaryFromBackpatch(&rhsExpr, sc);

		// Type check
		er->type = castAdditiveExpression(&lhsExpr, &rhsExpr, sc, action == actionSub);
		cleanType(lhsExpr.type);

		// Set new value for er
		er->arg->type = argInstr;
		er->arg->i = &arithProg->val;

		er->retType = exprRetRegular;

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

		er->retType = exprRetRegular;

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

		er->retType = exprRetRegular;
		
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
