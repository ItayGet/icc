// Lexer

#pragma once

#include "token.h"

// Generates a syntax error
void syntaxError(const char *expected, const char *got, int line);

typedef struct {
	// A function which gets the next character from the input stream
	int (*getNextChar)(); 

	// Retrieve last popped character back to input stream
	void (*pushLastChar)(int);
} Stream;

// Get next token in input stream
void getNextToken(Token *t, Stream *s);
