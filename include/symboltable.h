// Symbol table, implemented as a hash map

#pragma once

#include "symbol.h"

#define BUCKETS 4

typedef struct TableEntry {
	char *key;
	Symbol *value;
	struct TableEntry *next;
} TableEntry;

typedef struct {
	TableEntry *entries[BUCKETS];
}  SymbolTable;

void makeSymbolTable(SymbolTable *st);

void cleanSymbolTable(SymbolTable *st);

void cleanTableEntry(TableEntry *te);

TableEntry **getEntrySymbolTable(SymbolTable *st, char *key);

void insertEntrySymbolTable(SymbolTable *st, char *key, Symbol *value);

Symbol *getValueSymbolTable(SymbolTable *st, char *key);

void deleteEntrySymbolTable(SymbolTable *st, char *key);
