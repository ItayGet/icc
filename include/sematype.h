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
