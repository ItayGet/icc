// Defines a symbol that is held on the symbol table

#pragma once

#include "type.h"

// Type of each symbol
typedef enum {
	symbolError,
	symbolKeyword,
	symbolType,
	symbolFunction,
	symbolVariable,
} SymbolType;

// Id for each keyword
typedef enum {
	keywordIf,
	keywordElse,
} KeywordId;

typedef struct {
	SymbolType type;
	union {
		struct {
			KeywordId id;
		} keyword;

		Type typeS;

		struct {
			const char* name;
		} function;

		struct {
			Type *type;
			const char* name;
		} variable;
	};
} Symbol;
