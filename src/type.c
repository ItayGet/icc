#include "type.h"

#include <stdbool.h>
#include <stdlib.h>

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
	if(a == b) { return true; }

	if(a->type != b->type) { return false; }
	
	if(IS_BASIC_TYPE(a->type)) { return true; }

	switch(a->type) {
	case typeRecord:
		// a's address is not the same as b's meaning they are not the same struct
		return false;
	case typePointer:
		return areTypesEqual(a->pointer.type, b->pointer.type);
	case typeArray:
		return a->array.length == b->array.length &&
			areTypesEqual(a->array.type, b->array.type);
	}
}
