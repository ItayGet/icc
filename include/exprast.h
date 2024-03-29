// A represntation of an abstract syntax tree for expressions

#pragma once

#include <stdbool.h>

#include "type.h"

// An ordered enum denoting expression's operator precendence
typedef enum {
	precComma,
	precAssignment,
	precConditional,
	precLogicalOr,
	precLogicalAnd,
	precBitwiseOr,
	precBitwiseXor,
	precBitwiseAnd,
	precEquality,
	precRelational,
	precShift,
	precAdditive,
	precMultiplicative,
	precCast,
	precUnary,
	precPostfix,
} OperatorPrec;

typedef enum {
	exprAstIdentifier, // a
	exprAstConstant, // 1
	exprAstStringLiteral, // "Hello world"
	exprAstIndex, // a[i]
	exprAstCall, // func(a, b)
	exprAstMemberAccess, // a.b
	exprAstUnary, // a++, ++a, ~a
	exprAstCompoundLiteral, // (struct point) { 1, 2 }
	exprAstSizeofUnary, // sizeof a
	exprAstSizeofTypename, // sizeof(int)
	exprAstCast, // (int) a
	exprAstBinary, // a + b, a * b, a | b, a || b, a = b, a += b, a, b
	exprAstConditional, // a ? b : c
} ExprAstType;

typedef enum {
	exprAstPostfixPlus, // a++
	exprAstPostfixMinus, // a--
	exprAstPrefixPlus, // ++a
	exprAstPrefixMinus, // --a
	exprAstRef, // &a
	exprAstDeref, // *a
	exprAstUnaryPlus, // +a
	exprAstUnaryMinus, // -a
	exprAstBitwiseNot, // ~a
	exprAstLogicalNot, // !a
} ExprAstUnaryOp;

typedef enum {
	exprAstMultiply, // a * b
	exprAstDivide, // a / b
	exprAstModulo, // a % b
	exprAstAdd, // a + b
	exprAstSubract, // a - b
	exprAstShiftLeft, // a << b
	exprAstShiftRight, // a >> b
	exprAstLessThan, // a < b
	exprAstGreaterThan, // a > b
	exprAstLessThanEq, // a <= b
	exprAstGreaterThanEq, // a >= b
	exprAstEqual, // a == b
	exprAstNotEqual, // a != b
	exprAstBitwiseAnd, // a & b
	exprAstBitwiseXor, // a ^ b
	exprAstBitwiseOr, // a | b
	exprAstLogicalAnd, // a && b
	exprAstLogicalOr, // a || b
	exprAstRegAssignment, // a = b
	exprAstMultiplyAssignment, // a *= b
	exprAstDivideAssignment, // a /= b
	exprAstModuloAssignment, // a %= b
	exprAstAddAssignment, // a += b
	exprAstSubtractAssignment, // a -= b
	exprAstShiftLeftAssignment, // a <<= b
	exprAstShiftRightAssignment, // a >>= b
	exprAstBitwiseAndAssignment, // a &= b
	exprAstBitwiseXorAssignment, // a ^= b
	exprAstBitwiseOrAssignment, // a |= b
	exprAstComma, // a, b
} ExprAstBinaryOp;

typedef struct  ExprAst {
	ExprAstType type;
	
	Type *typename;

	union {
		// TODO: Finish the structs
		struct {
			// TODO: Temporary solution
			char *name;
		} identifier;

		struct {
			// Finish struct
		} constant;

		struct {
			// Finish struct
		} stringLiteral;

		struct {
			struct ExprAst *array, *index;
		} index;

		struct {
			// Finish struct
		} call;

		struct {
			struct ExprAst *record;
			bool isArrow;
			
			// TODO: Temporary solution
			char offset;
		} memberAccess;

		struct {
			struct ExprAst *operand;
			ExprAstUnaryOp op;
		} unary;

		struct {
			// Finish struct
		} compoundLiteral;

		struct {
			struct ExprAst *expr;
		} sizeofUnary;

		struct {
			// TODO: Temporary solution
			char *typename;
		} sizeofTypename;

		struct {
			struct ExprAst *expr;

			// TODO: Temporary solution
			char *typename;

		} cast;

		struct {
			struct ExprAst *lhs, *rhs;
			ExprAstBinaryOp op;
		} binary;

		struct {
			struct ExprAst *cond, *lhs, *rhs;
		} conditional;
	};
} ExprAst;
