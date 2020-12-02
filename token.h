// Token

#pragma once

typedef enum {
	tokenError,
	tokenEOF,
	tokenIdentifier,
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
	};
} Token;
