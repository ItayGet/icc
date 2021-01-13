// Token

#pragma once

#include "constant.h"

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

		Constant constant;
	};
} Token;
