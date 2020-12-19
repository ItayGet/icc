// Main file, currently holds temporary code that uses the latest system that was implemented
// Currently used to test the symboltable

#include "symboltable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *a[] = {
	#define TYPE(NAME, STR) STR,
	#include "type-definitions.h"
};

void stringifyType(Type *t, char *s) {
	if(IS_BASIC_TYPE(t->type)) {
		strcpy(s, a[t->type]);
		return;
	}

	char *tStr;
	switch(t->type) {
	case typeRecord:
		sprintf(s, a[t->type], "struct");
		break;
	case typePointer:;
		tStr = malloc(sizeof(char) * 4096);
		stringifyType(t->pointer.type, tStr);
		sprintf(s, a[t->type], tStr);
		free(tStr);
		break;
	case typeArray:;
		tStr = malloc(sizeof(char) * 4096);
		stringifyType(t->array.type, tStr);
		sprintf(s, a[t->type], tStr, t->array.length);
		free(tStr);
		break;
	}
}

void printType(Type *t) {
	char *s = malloc(sizeof(char) * 4096);
	stringifyType(t, s);
	printf("%s", s);
	free(s);

}

void printSymbol(Symbol *s) {
	switch(s->type) {
	case symbolType:
		printType(s->typeS);
		break;
	case symbolFunction:
		printf("%s", s->function.name);
		break;
	case symbolVariable:
		printf("var of type ");
		printType(s->variable.type);
		break;
	}
}

void printSymbolTable(SymbolTable *st) {
	for(int i = 0; i < BUCKETS; i++) {
		TableEntry *entry = st->entries[i];

		if(!entry) { continue; }

		printf("%d = \n", i);

		do {
			printf("	key: %s, value: ", entry->key);
			printSymbol(entry->value);
			printf("\n");

			entry = entry->next;
		}while(entry);
	}
}

#define FAST_STR(VARNAME, STR) char *VARNAME = malloc(sizeof(char) * 4096); strcpy(VARNAME, STR);

int main() {
	SymbolTable st;
	makeSymbolTable(&st);

	SymbolTable st2;
	makeSymbolTable(&st2);

	Symbol* s;

	FAST_STR(intStr, "int");
	Type *intT = malloc(sizeof(Type));
	makeType(intT);
	intT->type = typeInt;
	s = malloc(sizeof(Symbol));
	s->type = symbolType;
	s->typeS = intT;


	insertEntrySymbolTable(&st, intStr, s);

	FAST_STR(xStr, "x");
	s = malloc(sizeof(Symbol));
	s->type = symbolVariable;
	s->variable.type  = intT;
	increaseReferences(intT);

	insertEntrySymbolTable(&st, xStr, s);

	FAST_STR(ptrStr, "ptr");
	s = malloc(sizeof(Symbol));
	Type *ptrT = malloc(sizeof(Type));
	makeType(ptrT);
	ptrT->type = typePointer;
	increaseReferences(intT);
	ptrT->pointer.type = intT;
	s->type = symbolVariable;
	s->variable.type = ptrT;

	insertEntrySymbolTable(&st2, ptrStr, s);

	FAST_STR(arrStr, "arr");
	s = malloc(sizeof(Symbol));
	Type *arrT = malloc(sizeof(Type));
	makeType(arrT);
	arrT->type = typeArray;
	increaseReferences(intT);
	arrT->array.type = intT;
	arrT->array.length = 5;
	s->type = symbolVariable;
	s->variable.type = arrT;

	insertEntrySymbolTable(&st2, arrStr, s);

	puts("Here hold this");
	printSymbol(getValueSymbolTable(&st2, "arr"));
	puts("");
	printSymbol(getValueSymbolTable(&st, "x"));
	puts("");
	printSymbol(getValueSymbolTable(&st, "int"));
	puts("");

	puts("st:");
	printSymbolTable(&st);
	puts("st2:");
	printSymbolTable(&st2);

	cleanSymbolTable(&st);

	puts("Here hold this too");
	printSymbol(getValueSymbolTable(&st2, "ptr"));
	puts("");


	cleanSymbolTable(&st2);
}
