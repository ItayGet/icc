#include "exprparser.h"

#include <stdlib.h>
#include <stdbool.h>

// A helper function of parseCastExpression that parses the operations of
// postfix expression
ExprAst *parsePostfixExpression(TokenStream *ts) {

	Token t;
	getNextToken(&t, ts);

	// Expression should always hold the newest operation parsed
	ExprAst *expression = malloc(sizeof(ExprAst));
	expression->type = exprAstIdentifier;
	expression->identifier.name = t.identifier.name;

	while(true) {
		getNextToken(&t, ts);

		if(t.type != tokenPunctuator) {
			pushBackToken(ts, &t);
			return expression;
		}

		switch(t.punctuator.c) {
		case puncLSBracket:
			// ExprAst *index = parseExpression(ts);

			getNextToken(&t, ts);
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
			getNextToken(&t, ts);
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
			expression->type = exprAstUnary;
			expression->unary.operand = operand;
			expression->unary.op =
				t.punctuator.c == puncDPlus ?
				exprAstPostfixPlus :
				exprAstPostfixMinus;

			// TODO: Type checking and assign a type

			break;
		default: 
			pushBackToken(ts, &t);
			return expression;
		}
	}
}

// A helper function of parseCastExpression that parses unary expressions
ExprAst *parseUnaryExpression(TokenStream *ts, punctuatorType punc) {
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
	ExprAst *operand = parseCastExpression(ts);

	ExprAst *operation = malloc(sizeof(ExprAst));
	operation->type = exprAstUnary;
	operation->unary.operand = operand;
	operation->unary.op = op;

	// TODO: Type checking and assign a type
	
	return operation;
}

ExprAst *parseCastExpression(TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	switch(t.type) {
	// Since all binary expressions were or will be parsed out by binary
	// expression, this case has to be a primary expression with extra
	// stuff at the end which will get parsed out at the postfix expression
	case tokenIdentifier:
		// TODO: Check if identifier happens to be sizeof
	case tokenStringLiteral:
	case tokenIntegerConstant:
		pushBackToken(ts, &t);
		return parsePostfixExpression(ts);
		break;
	
	// This case is either for matching a unary expression or the
	// expressions beginning in a parenthesis
	case tokenPunctuator:

		// The case for FIRST = '(' is rather complex and shouldn't be
		// handled by parseUnaryExpression
		if(t.punctuator.c == puncLRBracket) {
			// TODO: Fill in
		}

		
		return parseUnaryExpression(ts, t.punctuator.c);

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

	case puncCaret:
		return precBitwiseXor;
		
	case puncBar:
		return precBitwiseOr;

	case puncDAmp:
		return precLogicalAnd;
	
	case puncDBar:
		return precLogicalOr;

	case puncEqual:
	case puncEAsterisk:
	case puncESlash:
	case puncEPercent:
	case puncEPlus:
	case puncEMinus:
	case puncDELT:
	case puncDEGT:
	case puncEAmp:
	case puncECaret:
	case puncEBar:
		return precAssignment;
	
	default:
		return BIN_OP_ERR_VALUE;
	}


#undef BIN_OP_ERR_VALUE
}

ExprAst *parseBinaryExpression(TokenStream *ts, OperatorPrec prec) {
	// Precedence not in range of binary expression
	if(prec > precEquality || prec < precCast) { /* error */ }

	// If an operand isn't a binary expression, the appropriate function
	// should be called
#define PARSE_OPERAND_EXPRESSION(ts, prec)    \
	((prec) == precMultiplicative)       \
	? parseCastExpression((ts))           \
	: parseBinaryExpression((ts), (prec)+1) \
	
	ExprAst *oper = PARSE_OPERAND_EXPRESSION(ts, prec);

	// Parse multiple operations of the same operator precedence
	while(true) {
		Token t;
		getNextToken(&t, ts);

		if(convertTokenToBinaryOperatorPrec(&t) != prec) {
			pushBackToken(ts, &t);
			return oper;
		}

		ExprAst *rhs = PARSE_OPERAND_EXPRESSION(ts, prec);

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
		case puncCaret:
			op = exprAstBitwiseXor;
			break;
		case puncBar:
			op = exprAstBitwiseOr;
			break;
		case puncDAmp:
			op = exprAstLogicalAnd;
			break;
		case puncDBar:
			op = exprAstLogicalOr;
			break;
		case puncEqual:
			op = exprAstRegAssignment;
			break;
		case puncEAsterisk:
			op = exprAstMultiplyAssignment;
			break;
		case puncESlash:
			op = exprAstDivideAssignment;
			break;
		case puncEPercent:
			op = exprAstModuloAssignment;
			break;
		case puncEPlus:
			op = exprAstAddAssignment;
			break;
		case puncEMinus:
			op = exprAstSubtractAssignment;
			break;
		case puncDELT:
			op = exprAstShiftLeftAssignment;
			break;
		case puncDEGT:
			op = exprAstShiftRightAssignment;
			break;
		case puncEAmp:
			op = exprAstBitwiseAndAssignment;
			break;
		case puncECaret:
			op = exprAstBitwiseXorAssignment;
			break;
		case puncEBar:
			op = exprAstBitwiseOrAssignment;
			break;

		default: /* error */;
		}

		oper->binary.op = op;

		// TODO: Type checking and assign a type
	}
#undef PARSE_LOWER_EXPRESSION
}
