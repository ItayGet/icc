// Lexer

#pragma once

#include "token.h"

typedef struct {
	// A function which gets the next character from the input stream
	int (*getNextChar)(); 

	// Retrieve last popped character back to input stream
	void (*pushLastChar)(int);
} Stream;

typedef struct {
	Stream s;

	// Is null when type is tokenError
	Token t;
} TokenStream;

void makeTokenStream(TokenStream *ts);

void getNextToken(Token *t, TokenStream *ts);

void pushBackToken(TokenStream *ts, Token *t);
