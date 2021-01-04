// A stream of tokens that allows to push tokens back once

#pragma once

#include "token.h"
#include "lex.h"

typedef struct {
	Stream s;

	// Is null when type is tokenError
	Token t;
} TokenStream;

void makeTokenStream(TokenStream *ts);

void getNextToken(Token *t, TokenStream *ts);

void pushBackToken(TokenStream *ts, Token *t);
