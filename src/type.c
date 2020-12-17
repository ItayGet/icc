#include "type.h"

#include <stdbool.h>

bool areTypesEqual(Type *a, Type *b) {
	if(a == b) { return true; }

	if(a->type != b->type) { return false; }
	
	if(IS_BASIC_TYPE(a->type)) { return true; }

	switch(a->type) {
	case typeRecord:
		// a's address is not the same as b's meaning they are not the same struct
		return false;
	case typePointer:
		return areTypesEqual(&a->pointer.type, &b->pointer.type);
	case typeArray:
		return a->array.length == b->array.lenth && areTypesEqual(&a->array.type, &b->array.type);
	}
}
