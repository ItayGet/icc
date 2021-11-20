// The type checking part of parsing

#pragma once

#include "exprast.h"
#include "type.h"

#include <stdbool.h>

#define IS_ANY_TYPE_EQUAL(LHS, RHS, TYPE) ((LHS) == (TYPE) || (RHS) == (TYPE))

// As defined in C99 6.3.1.1p2
BasicType promoteInteger(BasicType a, BasicRank *rank);

// Helper function of getUsualArithConversion
BasicType getUsualArithSingedUnsigned(BasicType signedType, BasicType unsignedType, int sRank, int uRank);

// Helper function of doUsualArithConversion
BasicType getUsualArithConversion(BasicType a, BasicType b);

// All binary cast operations clean rhs and might use the buffer to return
// while lhs stays unchanged and will not be cleaned

// As defined in C99 6.3.1.8
// lhs->type->type rhs->type->type should be typeBasic
Type *doUsualArithConversion(ExprRet *lhs, ExprRet *rhs, ScopeContext *sc);

void castAssignmentExpression(ExprRet *rhs, ScopeContext *sc, Type *lhsType);

Type *castAdditiveExpression(ExprRet *lhs, ExprRet *rhs, ScopeContext *sc, bool isSub);
