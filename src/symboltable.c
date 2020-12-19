#include "symboltable.h"

#include <string.h>
#include <stdlib.h>

int hash(char *key) {
	int len = strlen(key);
	long sum = 0;

	for(int i = 0; i < len; i++) {
		sum *= 41;
		sum += key[i];
	}

	return sum % BUCKETS;
}

void makeSymbolTable(SymbolTable *st) {
	for(int i = 0; i < BUCKETS; i++) {
		st->entries[i] = (TableEntry*)0;
	}
}

void cleanSymbolTable(SymbolTable *st) {
	for(int i = 0; i < BUCKETS; i++) {
		cleanTableEntry(st->entries[i]);
	}
}

void cleanTableEntry(TableEntry *te) {
	if(!te) { return; }

	free(te->key);
	cleanSymbol(te->value);

	cleanTableEntry(te->next);
	free(te);
}

TableEntry **getEntrySymbolTable(SymbolTable *st, char *key) {
	for(TableEntry **te = &st->entries[hash(key)]; *te; te = &(*te)->next) {
		if(!strcmp((*te)->key, key)) {
			return te;
		}
	}
	// Value not found
	return NULL;
}

void insertEntrySymbolTable(SymbolTable *st, char *key, Symbol *value) {
	int index = hash(key);

	TableEntry *newTe = malloc(sizeof(TableEntry)),
		   *oldTe = st->entries[index];

	st->entries[index] = newTe;

	newTe->key = key;
	newTe->value = value;
	newTe->next = oldTe;
}

Symbol *getValueSymbolTable(SymbolTable *st, char *key) {
	TableEntry **entry = getEntrySymbolTable(st, key);

	if(!entry) { return NULL; }

	return (*entry)->value;
}

void deleteEntrySymbolTable(SymbolTable *st, char *key) {
	TableEntry **entry = getEntrySymbolTable(st, key);

	if(!entry) { return; }

	TableEntry *oldEntry = *entry;

	*entry = oldEntry->next;

	oldEntry->next = NULL;
	cleanTableEntry(oldEntry);
}
