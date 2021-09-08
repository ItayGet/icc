#include "exprparser.h"

#include <stdlib.h>
#include <stdbool.h>

// A helper function of parseCastExpression that parses the operations of
// postfix expression
ExprAst *parsePostfixExpression(Stream *s, ExprAst *primaryExpression) {
	// Expression should always hold the newest operation parsed
	ExprAst *expression = primaryExpression;

	while(true) {
		Token t;
		getNextToken(&t, s);

		if(t.type != tokenPunctuator) {
			return expression;
		}

		switch(t.punctuator.c) {
		case puncLSBracket:
			// ExprAst *index = parseExpression(s);

			getNextToken(&t, s);
			if(t.type != tokenPunctuator ||
			   t.punctuator.c != puncRSBracket) {/* error */}

			ExprAst *array = expression;
			expression = malloc(sizeof(ExprAst));
			expression->type = exprAstIndex;
			expression->index.array = array;
			// primaryExpression->index.index = index;

			// TODO: Type checking and assign a type

			break;
		case puncLRBracket:
			// TODO: Function call
		case puncArrow:;
		case puncDot:
			getNextToken(&t, s);
			if(t.type != tokenIdentifier) {/* error */}

			// TODO: Use symbols to find the offset in the record
			
			ExprAst *record = expression;
			expression = malloc(sizeof(ExprAst));
			expression->type = exprAstMemberAccess;
			expression->memberAccess.record = record;
			expression->memberAccess.offset = 0;
			expression->memberAccess.isArrow = 
				t.punctuator.c == puncArrow;

			// TODO: Type checking and assign a type

			break;
		case puncDPlus:
		case puncDMinus:;
			ExprAst *operand = expression;
			expression = malloc(sizeof(ExprAst));
			operand->type = exprAstUnary;
			operand->unary.operand = operand;
			operand->unary.op =
				t.punctuator.c == puncDPlus ?
				exprAstPostfixPlus :
				exprAstPostfixMinus;

			// TODO: Type checking and assign a type

			break;
		default: return expression;
		}
	}
}

// A helper function of parseCastExpression that parses unary expressions
ExprAst *parseUnaryExpression(Stream *s, punctuatorType punc) {
	ExprAstUnaryOp op;
	switch(punc) {
	case puncAmp:
		op = exprAstRef;
		break;
	case puncAsterisk:
		op = exprAstDeref;
		break;
	case puncPlus:
		op = exprAstUnaryPlus;
		break;
	case puncMinus:
		op = exprAstUnaryMinus;
		break;
	case puncTilde:
		op = exprAstBitwiseNot;
		break;
	case puncExcl:
		op = exprAstLogicalNot;
		break;
	case puncDPlus:
		op = exprAstPrefixPlus;
		break;
	case puncDMinus:
		op = exprAstPrefixMinus;
		break;

	// Should return back to the previous expression for handling the error
	default: /* error */;
	}

	// Parse rest of the expression, which has to be a cast expression
	// In case of if the cast expression begins with a prefix de/increment,
	// it will give off a semantic error instead of a syntactic error
	ExprAst *operand = parseCastExpression(s);

	ExprAst *operation = malloc(sizeof(ExprAst));
	operation->type = exprAstUnary;
	operation->unary.operand = operand;
	operation->unary.op = op;

	// TODO: Type checking and assign a type
	
	return operation;
}

ExprAst *parseCastExpression(Stream *s) {
	Token t;
	getNextToken(&t, s);

	switch(t.type) {
	// Since all binary expressions were or will be parsed out by binary
	// expression, this case has to be a primary expression with extra
	// stuff at the end which will get parsed out at the postfix expression
	case tokenIdentifier:
		// TODO: sizeof operator
	case tokenStringLiteral:
	case tokenIntegerConstant:
		// TODO: Fill in
		parsePostfixExpression(s, /* tree with the right token */);
		break;
	
	// This case is either for matching a unary expression or the
	// expressions beginning in a parenthesis
	case tokenPunctuator:

		// The case for FIRST = '(' is rather complex and shouldn't be
		// handled by parseUnaryExpression
		if(t.punctuator.c == puncLRBracket) {
			// TODO: Fill in
		}

		
		return parseUnaryExpression(s, t.punctuator.c);

		break;
	default: /* error */;
	}	
}

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
		oper->binary.lhs = lhs;
		oper->binary.rhs = rhs;

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

		oper->binary.op = op;

		// TODO: Type checking and assign a type
	}
#undef PARSE_LOWER_EXPRESSION
}
