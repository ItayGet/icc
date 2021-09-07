#include "exprparser.h"

#include <stdlib.h>
#include <stdbool.h>

// A helper function for parseBinaryExpression that takes a token and returns
// the right binary operator precendence
OperatorPrec convertTokenToBinaryOperatorPrec(Token *t) {
	// TODO: Add an error value to enums at exprast
#define BIN_OP_ERR_VALUE -1

	if(t->type != tokenPunctuator) { return BIN_OP_ERR_VALUE; }

	switch(t->punctuator.c) {
	case puncAsterisk:
	case puncSlash:
	case puncPercent:
		return precMultiplicative;
	
	case puncPlus:
	case puncMinus:
		return precAdditive;

	case puncDLT:
	case puncDGT:
		return precShift;
	
	case puncLT:
	case puncGT:
	case puncELT:
	case puncEGT:
		return precRelational;

	case puncDEqual:
	case puncEExcl:
		return precEquality;	
		
	case puncAmp:
		return precBitwiseAnd;

	case puncBar:
		return precBitwiseOr;

	case puncCaret:
		return precBitwiseXor;
		
	default:
		return BIN_OP_ERR_VALUE;
	}


#undef BIN_OP_ERR_VALUE
}

ExprAst *parseBinaryExpression(Stream *s, OperatorPrec prec) {
	// Precedence not in range of binary expression
	if(prec > precBitwiseOr || prec < precCast) { /* error */ }

	// If an operand isn't a binary expression, the appropriate function
	// should be called
#define PARSE_OPERAND_EXPRESSION(s, prec)    \
	((prec) == precMultiplicative)       \
	? parseCastExpression((s))           \
	: parseBinaryExpression((s), (prec)+1) \
	
	ExprAst *oper = PARSE_OPERAND_EXPRESSION(s, prec);

	// Parse multiple operations of the same operator precedence
	while(true) {
		Token t;
		getNextToken(&t, s);

		if(convertTokenToBinaryOperatorPrec(&t) != prec) {
			return oper;
		}

		ExprAst *rhs = PARSE_OPERAND_EXPRESSION(s, prec);

		// update oper so it will hold the new operation
		ExprAst *lhs = oper;
		oper = malloc(sizeof(ExprAst));
		oper->type = exprAstBinary;
		oper->Binary.lhs = lhs;
		oper->Binary.rhs = rhs;

		// Decide the operator for the new operation
		ExprAstBinaryOp op;
		switch(t.punctuator.c) {
		case puncAsterisk:
			op = exprAstMultiply;
			break;
		case puncSlash:
			op = exprAstDivide;
			break;
		case puncPercent:
			op = exprAstModulo;
			break;
		case puncPlus:
			op = exprAstAdd;
			break;
		case puncMinus:
			op = exprAstSubract;
			break;
		case puncDLT:
			op = exprAstShiftLeft;
			break;
		case puncDGT:
			op = exprAstShiftRight;
			break;
		case puncLT:
			op = exprAstGreaterThan;
			break;
		case puncGT:
			op = exprAstLessThan;
			break;
		case puncELT:
			op = exprAstGreaterThanEq;
			break;
		case puncEGT:
			op = exprAstLessThanEq;
			break;
		case puncDEqual:
			op = exprAstEqual;
			break;
		case puncEExcl:
			op = exprAstNotEqual;
			break;
		case puncAmp:
			op = exprAstBitwiseAnd;
			break;
		case puncBar:
			op = exprAstBitwiseOr;
			break;
		case puncCaret:
			op = exprAstBitwiseXor;
			break;
		default: /* error */;
		}

		oper->Binary.op = op;

		// TODO: Type checking and assign a type
	}
#undef PARSE_LOWER_EXPRESSION
}
