// The type checking part of parsing

#pragma once

#include "parser.h"
#include "type.h"

#define IS_ANY_TYPE_EQUAL(LHS, RHS, TYPE) ((LHS) == (TYPE) || (RHS) == (TYPE))

// Cast a basic type
void castIrArgBasicType(IrArg *arg, ScopeContext *sc, BasicType curr, BasicType castType);

// As defined in C99 6.3.1.1p2
BasicType promoteInteger(BasicType a, BasicRank *rank);

// Helper function of getUsualArithConversion
BasicType getUsualArithSingedUnsigned(BasicType signedType, BasicType unsignedType, int sRank, int uRank);

// Helper function of doUsualArithConversion
BasicType getUsualArithConversion(BasicType a, BasicType b);

// As defined in C99 6.3.1.8
// lhs and rhs should be typeBasic
// when returning, lhs should stay the same and not be cleaned while rhs will
// be changed to the new type and returned
// If you don't need the value of lhs anymore, you should clean it yourself
Type *doUsualArithConversion(Type *lhs, Type *rhs);

void castAssignmentExpression(ExprRet *rhs, ScopeContext *sc, Type *lhsType);

Type *castAddExpression(ExprRet *lhs, ExprRet *rhs, ScopeContext *sc);
