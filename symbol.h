// Defines a symbol that is held on the symbol table

#pragma once

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

		struct {
			const char* name;
		} type;

		struct {
			const char* name;
		} function;

		struct {
			const char* name;
			Symbol type;
		} variable;
	}
} Symbol;
