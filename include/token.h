// Token
#pragma once

typedef enum {
	tokenError,
	tokenEOF,
	tokenIdentifier,
	tokenIntegerConstant,
	tokenStringLiteral,
	tokenPunctuator,
} TokenType;

// Punctuator types sometimes have values that exceed the limits of chars
typedef enum {
	#define TOKEN(NAME) NAME,
	#include "token.def"
} PunctuatorType;

typedef struct {
	TokenType type;
	union {
		struct {
			char* name;
		} identifier;

		struct {
			PunctuatorType c;
		} punctuator;

		struct {
			char* contents;
		} stringLiteral;

		struct {
			int value;
		} integer;
	};
} Token;
