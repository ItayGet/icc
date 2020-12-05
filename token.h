// Token

#pragma once

typedef enum {
	tokenError,
	tokenEOF,
	tokenIdentifier,
	tokenIntegerConstant,
	tokenStringLiteral,
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
			char* contents;
		} stringLiteral;

		struct {
			int value;
		} integer;
	};
} Token;
