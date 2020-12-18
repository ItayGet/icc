// Symbol table, implemented as a hash map

#pragma once

#include "symbol.h"

#define BUCKETS 128

typedef struct TableEntry {
	char *key;
	Symbol *value;
	struct TableEntry *next;
} TableEntry;

typedef struct {
	TableEntry *entries[BUCKETS];
}  SymbolTable;

void makeSymbolTable(SymbolTable *st);

void insertEntrySymbolTable(SymbolTable *st, char *key, Symbol *value);

Symbol *getEntrySymbolTable(SymbolTable *st, char *key);

void deleteEntrySymbolTable(SymbolTable *st, char *key);
