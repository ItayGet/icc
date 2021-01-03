#include "type.h"

#include <stdbool.h>
#include <stdlib.h>

BasicTypeType typeValuesArr[] = {
	#define BASIC_TYPE(NAME, STR) basicTypeError,
	#define SIGNED_TYPE(NAME, STR) basicTypeSigned,
	#define UNSIGNED_TYPE(NAME, STR) basicTypeUnsigned,
	#define FLOAT_TYPE(NAME, STR) basicTypeFloat,
	#include "type.def"
};

void makeType(Type *t) {
	t->references = 1;
}

void increaseReferences(Type *t) {
	t->references++;
}

void cleanType(Type *t) {
	t->references--;

	if(t->references > 0) { return; }

	switch(t->type) {
	case typeRecord:;
		RecordMember *rm = t->record.member;
		while(rm) {
			cleanType(rm->type);

			RecordMember *temp = rm;
			
			rm = rm->next;

			free(temp);
		}
		break;
	case typePointer:
		cleanType(t->pointer.type);
		break;
	case typeArray:
		cleanType(t->array.type);
		break;
	}
	free(t);
}

int getBasicTypeRank(BasicType basic) {
	switch(basic) {
	case basicChar:
	case basicUChar:
		return 1;
	case basicSInt:
	case basicUSInt:
		return 2;
	case basicInt:
	case basicUInt:
		return 3;
	case basicLInt:
	case basicULInt:
		return 4;
	case basicLLInt:
	case basicULLInt:
		return 5;
	case basicFloat:
		return 6;
	case basicDouble:
		return 7;
	case basicLDouble:
		return 8;
	default:
		return -1;
	}
}

BasicType doUsualArithConversion(BasicType a, BasicType b) {
	BasicTypeType ta = typeValuesArr[a], tb = typeValuesArr[b];

	if(ta == tb) {
		return a;
	}

	if(ta == basicTypeError || tb == basicTypeError) { /* error */ }

	int aRank = getBasicTypeRank(a);
	int bRank = getBasicTypeRank(b);

	if(ta == basicTypeFloat || tb == basicTypeFloat) {
		return a ? aRank > bRank : b;
	}

	BasicType ret;
	// By deducing the state of the function, if a is signed then b is
	// unsigned. Otherwise, b is signed
	if(ta == basicTypeSigned) {
		ret = doUsualArithInt(a, b, aRank, bRank);
	} else {
		ret = doUsualArithInt(b, a, bRank, aRank);
	}
	return ret;
}

BasicType getUnignedBasicType(BasicType signedType) {
	switch(signedType) {
	case basicChar:
		return basicUChar;
	case basicSInt:
		return basicUSInt;
	case basicInt:
		return basicUInt;
	case basicLInt:
		return basicULInt;
	case basicLLInt:
		return basicULLInt;
	default:
		return basicError;
	}
}

BasicType doUsualArithInt(BasicType signedType, BasicType unsignedType,
			  int sRank, int uRank) {
	if(uRank >= sRank) {
		return unsignedType;
	}

	// FIXME: Use types' sizes to determine if the unsigned type could be
	// held inside the signed type
	return getUnignedBasicType(signedType);
}
