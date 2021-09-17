// Lexer

#pragma once

#include "token.h"

#include <stdio.h>

typedef enum {
	StreamError,
	StreamFile,
	StreamArray,
} StreamType;

typedef struct {
	StreamType type;

	union {
		struct {
			FILE *file;
		} file;

		struct {
			char *array;
			char *pointer;
		} array;
	};
} Stream;

typedef struct {
	Stream s;

	// Is null when type is tokenError
	Token t;
} TokenStream;

int getNextChar(Stream *s);

void pushBackChar(Stream *s, char c);

void cleanStream(Stream *s);

void makeTokenStream(TokenStream *ts);

void getNextToken(Token *t, TokenStream *ts);

void pushBackToken(TokenStream *ts, Token *t);

void consumePuncToken(TokenStream *ts, PunctuatorType punc);
