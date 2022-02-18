#include "sematype.h"

#include <stdlib.h>

BasicType promoteInteger(BasicType a, BasicRank *rank) {
	// Integer promotions don't change types whose ranks are bigger than
	// int
	if(*rank > GET_BASIC_RANK(basicInt)) { return a; }
	
	// FIXME: Check if all values of a can be represented by an int and if
	// they don't, return unsigned int
	*rank = GET_BASIC_RANK(basicInt);
	return a == basicUInt ? basicUInt : basicInt;
}

BasicType getUsualArithSingedUnsigned(BasicType signedType, BasicType unsignedType,
				      int sRank, int uRank) {
	if(uRank >= sRank) {
		return unsignedType;
	}

	// FIXME: Use types' sizes to determine if the unsigned type could be
	// held inside the signed type
	return getUnignedBasicType(signedType);
}

BasicType getUsualArithConversion(BasicType a, BasicType b) {
	BasicTypeType ta = GET_BASIC_TYPE_TYPE(a), tb = GET_BASIC_TYPE_TYPE(b);

	if(IS_ANY_TYPE_EQUAL(ta, tb, basicTypeError)) { /* error */ }
	
	BasicRank aRank = GET_BASIC_RANK(a);
	BasicRank bRank = GET_BASIC_RANK(b);

	if(IS_ANY_TYPE_EQUAL(ta, tb, basicTypeFloat)) {
		return aRank > bRank  ? a : b;
	}

	// Integer promotion rules
	a = promoteInteger(a, &aRank);
	b = promoteInteger(b, &bRank);

	if(ta == tb) {
		return aRank > bRank  ? a : b;
	}

	BasicType ret;
	// By deducing the state of the function, one type is signed and the
	// other is unsigned
	if(ta == basicTypeSigned) {
		ret = getUsualArithSingedUnsigned(a, b, aRank, bRank);
	} else {
		ret = getUsualArithSingedUnsigned(b, a, bRank, aRank);
	}
	return ret;
}
