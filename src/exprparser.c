#include "exprparser.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// A helper function of parsePrimaryExpression that parses the end of a sizeof
// expression
ExprAst *parseRestOfSizeOfExpression(TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	// HACK: In case the expression starts with parentheses but is a unary
	// expression this will make guarantee that the right parenthesis will
	// be consumed
	bool isParenOpen = false;
	if(t.type == tokenPunctuator && t.punctuator.c == puncLRBracket) {
		// TODO: Because a type system wasn't implemented yet the
		// indetifier int will be considered a start of a type an the
		// only type
		getNextToken(&t, ts);

		if(t.type == tokenIdentifier && !strcmp(t.identifier.name, "int")) { 
			Token paren;
			getNextToken(&paren, ts);

			if(paren.type != tokenPunctuator || 
				paren.punctuator.c == puncLRBracket) { /* error */ }

			
			ExprAst *expression = malloc(sizeof(ExprAst));
			expression->type = exprAstSizeofTypename;
			expression->sizeofTypename.type = t.identifier.name;

			// TODO: Type checking and assign a type
			
			return expression;
		}

		pushBackToken(ts, &t);
		isParenOpen = true;
	} else { pushBackToken(ts, &t); }


	ExprAst *cast = parseCastExpression(ts);
	ExprAst *expression = malloc(sizeof(ExprAst));
	expression->type = exprAstSizeofUnary;
	expression->sizeofUnary.expr = cast;

	if(isParenOpen) {
		getNextToken(&t, ts);
		if(t.type != tokenPunctuator || 
			t.punctuator.c == puncLRBracket) { /* error */ }
	}

	// TODO: Type checking and assign a type
	
	return expression;
}

// A helper function of parsePostfixExpression that parses a primary expression
ExprAst *parsePrimaryExpression(TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	// TODO: Parse constants and string-literals

	// Check if identifier happens to be sizeof
	if(!strcmp(t.identifier.name, "sizeof")) {
		return parseRestOfSizeOfExpression(ts);
	}

	ExprAst *expression = malloc(sizeof(ExprAst));
	expression->type = exprAstIdentifier;
	expression->identifier.name = t.identifier.name;

	// TODO: Type checking and assign a type

	return expression;
}

// A helper function of parseCastExpression that parses the operations of
// postfix expression
ExprAst *parsePostfixExpression(TokenStream *ts, ExprAst *primaryExpression) {
	while(true) {
		Token t;
		getNextToken(&t, ts);

		if(t.type != tokenPunctuator) {
			pushBackToken(ts, &t);
			return primaryExpression;
		}

		switch(t.punctuator.c) {
		case puncLSBracket:;
			ExprAst *index = parseExpression(ts);

			getNextToken(&t, ts);
			if(t.type != tokenPunctuator ||
			   t.punctuator.c != puncRSBracket) {/* error */}

			ExprAst *array = primaryExpression;
			primaryExpression = malloc(sizeof(ExprAst));
			primaryExpression->type = exprAstIndex;
			primaryExpression->index.array = array;
			primaryExpression->index.index = index;

			// TODO: Type checking and assign a type

			break;
		case puncLRBracket:
			// TODO: Function call
		case puncArrow:;
		case puncDot:
			getNextToken(&t, ts);
			if(t.type != tokenIdentifier) {/* error */}

			// TODO: Use symbols to find the offset in the record
			
			ExprAst *record = primaryExpression;
			primaryExpression = malloc(sizeof(ExprAst));
			primaryExpression->type = exprAstMemberAccess;
			primaryExpression->memberAccess.record = record;
			primaryExpression->memberAccess.offset = 0;
			primaryExpression->memberAccess.isArrow = 
				t.punctuator.c == puncArrow;

			// TODO: Type checking and assign a type

			break;
		case puncDPlus:
		case puncDMinus:;
			ExprAst *operand = primaryExpression;
			primaryExpression = malloc(sizeof(ExprAst));
			primaryExpression->type = exprAstUnary;
			primaryExpression->unary.operand = operand;
			primaryExpression->unary.op =
				t.punctuator.c == puncDPlus ?
				exprAstPostfixPlus :
				exprAstPostfixMinus;

			// TODO: Type checking and assign a type

			break;
		default: 
			pushBackToken(ts, &t);
			return primaryExpression;
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

// A helper function of parseCastExpression that parses the parenthses part of
// expressions
ExprAst *parseParenthesesExpression(TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	// TODO: Because a type system wasn't implemented yet the
	// indetifier int will be considered a start of a type an the
	// only type
	if(t.type != tokenIdentifier || strcmp(t.identifier.name, "int")) {
		// an expression inside parentheses

		pushBackToken(ts, &t);

		ExprAst *expression = parseExpression(ts);
		
		getNextToken(&t, ts);
		if(t.type != tokenPunctuator ||
				t.punctuator.c != puncRRBracket) {/* error */}

		return parsePostfixExpression(ts, expression);
	}

	Token type = t;

	getNextToken(&t, ts);
	if(t.type != tokenPunctuator ||
			t.punctuator.c != puncRRBracket) {/* error */}

	getNextToken(&t, ts);

	
	if( t.type != tokenPunctuator || t.punctuator.c != puncLCBracket) {
		// Cast expression

		pushBackToken(ts, &t);
		ExprAst *expression = malloc(sizeof(ExprAst));
		expression->type = exprAstCast;
		expression->cast.expr = parseCastExpression(ts);
		expression->cast.type = type.identifier.name;

		return expression;
	}

	// TODO: Compount literals
}

ExprAst *parseCastExpression(TokenStream *ts) {
	Token t;
	getNextToken(&t, ts);

	switch(t.type) {
	// Since all binary expressions were or will be parsed out by binary
	// expression, this case has to be a primary expression with extra
	// stuff at the end which will get parsed out at the postfix expression
	case tokenIdentifier:
	case tokenStringLiteral:
	case tokenIntegerConstant:
		pushBackToken(ts, &t);
		ExprAst *primaryExpression = parsePrimaryExpression(ts);

		return parsePostfixExpression(ts, primaryExpression);
		break;
	
	// This case is either for matching a unary expression or the
	// expressions beginning in a parenthesis
	case tokenPunctuator:

		// The case for FIRST = '(' is rather complex and shouldn't be
		// handled by parseUnaryExpression
		if(t.punctuator.c == puncLRBracket) {
			return parseParenthesesExpression(ts);
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

	case puncComma:
		return precComma;
	
	default:
		return BIN_OP_ERR_VALUE;
	}


#undef BIN_OP_ERR_VALUE
}

ExprAst *parseBinaryExpression(TokenStream *ts, OperatorPrec prec) {
	// If an operand isn't a binary expression, the appropriate function
	// should be called
	if(prec == precCast) { return parseCastExpression(ts); }
	if(prec == precConditional) { return parseConditionalExpression(ts); }
	
	ExprAst *oper = parseBinaryExpression(ts, prec+1);

	// Parse multiple operations of the same operator precedence
	while(true) {
		Token t;
		getNextToken(&t, ts);

		if(convertTokenToBinaryOperatorPrec(&t) != prec) {
			pushBackToken(ts, &t);
			return oper;
		}

		ExprAst *rhs = parseBinaryExpression(ts, prec+1);

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
		case puncComma:
			op = exprAstComma;
			break;

		default: /* error */;
		}

		oper->binary.op = op;

		// TODO: Type checking and assign a type
	}
}

ExprAst *parseConditionalExpression(TokenStream *ts) {
	ExprAst *cond = parseBinaryExpression(ts, precLogicalOr);

	Token t;
	getNextToken(&t, ts);

	if(t.type != tokenPunctuator || t.punctuator.c != puncQue) {
		pushBackToken(ts, &t);
		return cond;
	}
	
	ExprAst *lhs = parseExpression(ts);


	getNextToken(&t, ts);

	if(t.type != tokenPunctuator || t.punctuator.c != puncColon) {
		/* error */
	}

	ExprAst *rhs = parseConditionalExpression(ts);

	ExprAst *expression = malloc(sizeof(ExprAst));
	expression->type = exprAstConditional;
	expression->conditional.cond = cond;
	expression->conditional.lhs = lhs;
	expression->conditional.rhs = rhs;

	// TODO: Type checking
	
	return expression;
}
