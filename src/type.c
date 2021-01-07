#include "type.h"

#include <stdbool.h>
#include <stdlib.h>

BasicTypeType basicTypeTypeValuesArr[] = {
	#define BASIC_TYPE(NAME, STR) basicTypeError,
	#define SIGNED_TYPE(NAME, STR) basicTypeSigned,
	#define UNSIGNED_TYPE(NAME, STR) basicTypeUnsigned,
	#define FLOAT_TYPE(NAME, STR) basicTypeFloat,
	#include "type.def"
};

BasicRank basicRankValuesArr[] = {
#define BASIC_TYPE(NAME, STR) __ ## NAME,
#define __Error -1
#define __Void 0
#define __Char 1
#define __UChar 1
#define __SInt 2
#define __USInt 2
#define __Int 3
#define __UInt 3
#define __LInt 4
#define __ULInt 4
#define __LLInt 5
#define __ULLInt 5
#define __Float 6
#define __Double 7
#define __LDouble 8
#include "type.def"
#undef __Error
#undef __Void
#undef __Char
#undef __UChar
#undef __SInt
#undef __USInt
#undef __Int
#undef __UInt
#undef __LInt
#undef __ULInt
#undef __LLInt
#undef __ULLInt
#undef __Float
#undef __Double
#undef __LDouble
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

bool areTypesEqual(Type *a, Type *b) {
	// TODO: What about qualifiers?

	if(a == b) { return true; }

	if(a->type != b->type) { return false; }

	switch(a->type) {
	case typeBasic:
		return a->basic != b->basic;
	case typeRecord:
		// a's address is not the same as b's meaning they are not the same struct
		return false;
	case typePointer:
		return areTypesEqual(a->pointer.type, b->pointer.type);
	case typeArray:
		return a->array.length == b->array.length && areTypesEqual(a->array.type, b->array.type);
	default:
		return false;
	}
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
