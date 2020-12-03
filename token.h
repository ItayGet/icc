// Token

#pragma once

typedef enum {
	tokenError,
	tokenEOF,
	tokenIdentifier,
	tokenIntegerConstant,
	tokenCharacter,
} TokenType;

typedef struct {
	TokenType type;
	union {
		struct {
			char* name;
		} identifier;

		struct {
			int c;
		} character;

		struct {
			int value;
		} integer;
	};
} Token;
