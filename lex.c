#include "lex.h"
#include "token.h"
#include "stringbuilder.h"
#include "literal.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define MIN_IDEN_SIZE 16 * sizeof(char)

void syntaxError(const char *expected, const char *got, int line) {
	fprintf(stderr, "Syntax Error: expected %s, but got '%s' at line %d\n", expected, got, line);
	exit(1);
}

// Pop all whitespace and comments
// Parameter c holds currently popped character from the stream
void skipWhitespace(Stream *s, int *c) {
	// Whether last iteration skipped characters
	bool success;

	// Skip multiple comments and whitespace
	do {
		success = false;
		
		// Skip whitespace
		while(true) {
			switch(*c) {
			case ' ':
			case '\t':
			case '\n':
				success = true;
				*c = s->getNextChar();
				continue;
			case EOF:;
				 // Pass EOF to parent function(getNextToken)
				 return;
			}
			break;
		}

		// Skip comments
		if(*c == '/') {
			*c = s->getNextChar();
			switch(*c) {
			// Single-line comment
			case '/':
				success = true;
				while((*c = s->getNextChar()) != '\n') {
					if(*c == EOF) { syntaxError("end of line", "EOF", 0); }
				}
				break;
			// Multi-line comment
			case '*':
				success = true;
				while(true) {
					*c = s->getNextChar();
					if(*c == EOF) {
						syntaxError("*/", "EOF", 0);
					}
					if(*c == '*') {
						*c = s->getNextChar();
						if(*c != '/') {
							s->pushLastChar(*c);
						}
						break;
					}
				}
				break;
			default:;
				char got[] = { *c, 0 };
				syntaxError("/", got, 0);
			}
			*c = s->getNextChar();
		}
	} while(success);
}

// Check if current token is an identifer
// If it is return true and change token
// else, return false
bool isTokenIdentifier(Token *t, Stream *s, int c) {
	if(!isalpha(c)) { return false; }

	StringBuilder sb;
	makeStringBuilder(&sb, CAP_INC_SB);

	do {
		appendStringBuilderChar(&sb, c);

		c = s->getNextChar();
	} while(isalnum(c) || c == '_');

	s->pushLastChar(c);

	stringifyStringBuilderBuffer(&sb);
	
	t->type = tokenIdentifier;
	t->identifier.name = sb.buffer;

	return true;
}

// Check if current token is an integer constant
// If it is return true and change token
// else, return false
bool isTokenIntegerConstant(Token *t, Stream *s, int c) {
	if(!isdigit(c)) { return false; }

	int value = 0;

	// Octal and hex
	if(c == '0') {
		c = s->getNextChar();
		if(c == 'x' || c == 'X') {
			// Hex
			c = s->getNextChar();
			//if(!isxdigit(c) { error }

			for(; true; c = s->getNextChar()) {
				if(c >= 'a' && c <= 'f') {
					value *= 16;
					value += c - 'a' + 10;
				}
				else if(c >= 'A' && c >= 'F') {
					value *= 16;
					value += c - 'A' + 10;
				}
				else if(isdigit(c)) {
					value *= 16;
					value += c - '0';
				}else {
					break;
				}
			} 
		} else {
			// Octal
			for(; c >= '0' && c <= '7'; c = s->getNextChar()) {
				value *= 8;
				value += c - '0';
			}
		}
	} else {
		// Decimal
		for(; isdigit(c); c = s->getNextChar()) {
			value *= 10;
			value += c - '0';
		}
	}

	// TODO: add UL suffixes
	// TODO: reform to add floats
	
	s->pushLastChar(c);

	t->type = tokenIntegerConstant;
	t->integer.value = value;

	return true;
}

void getNextToken(Token *t, Stream *s) {
	int c = s->getNextChar();

	skipWhitespace(s, &c);
	
	if(c == EOF) { 
		t->type = tokenEOF;
		return;
	}

	if(isTokenStringLiteral(t, s, c)) { return; }
	
	if(isTokenIdentifier(t, s, c)) { return; }

	if(isTokenIntegerConstant(t, s, c)) { return; }
}
