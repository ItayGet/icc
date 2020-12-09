// Token
#pragma once

#include <limits.h>

typedef enum {
	tokenError,
	tokenEOF,
	tokenIdentifier,
	tokenIntegerConstant,
	tokenStringLiteral,
	tokenPunctuator,
} TokenType;

// Used to create punctuators that exeed the limits of chars
#define OVERFLOW_CHAR(c) c + UCHAR_MAX + 1

// Punctuator types sometimes have values that exceed the limits of chars
typedef enum {
	#define TOKEN(NAME) NAME,
	#include "token-definitions.h"
} punctuatorType;

typedef struct {
	TokenType type;
	union {
		struct {
			char* name;
		} identifier;

		struct {
			punctuatorType c;
		} punctuator;

		struct {
			char* contents;
		} stringLiteral;

		struct {
			int value;
		} integer;
	};
} Token;
