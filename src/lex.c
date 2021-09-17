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
	if(!isalpha(c) && c != '_') { return false; }

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

// Check if current token is a punctuator
// If it is return true and change token
// else, return false
bool isTokenPunctuator(Token *t, Stream *s, int c) {
	switch(c) {
	// All characters that start a single token
	case '[':
		t->punctuator.c = puncLSBracket;
		break;
	case ']':
		t->punctuator.c = puncRSBracket;
		break;
	case '(':
		t->punctuator.c = puncLRBracket;
		break;
	case ')':
		t->punctuator.c = puncRRBracket;
		break;
	case '{':
		t->punctuator.c = puncLCBracket;
		break;
	case '}':
		t->punctuator.c = puncRCBracket;
		break;
	case '~':
		t->punctuator.c = puncTilde;
		break;
	case ',':
		t->punctuator.c = puncComma;
		break;
	case '.':
		t->punctuator.c = puncDot;
		break;
	case ';':
		t->punctuator.c = puncSemicolon;
		break;
	case '?':
		t->punctuator.c = puncQue;
		break;
	case ':':
	       t->punctuator.c = puncColon;
	       break;
	case '+':
		c = s->getNextChar();
		switch(c) {
		case '+':
			t->punctuator.c = puncDPlus;
			break;
		case '=':
			t->punctuator.c = puncEPlus;
			break;
		default:
			t->punctuator.c = puncPlus;
			s->pushLastChar(c);
			break;
		}
		break;
	case '-':
		c = s->getNextChar();
		switch(c) {
		case '-':
			t->punctuator.c = puncDMinus;
			break;
		case '=':
			t->punctuator.c = puncEMinus;
			break;
		case '>':
			t->punctuator.c = puncArrow;
		default:
			t->punctuator.c = puncMinus;
			s->pushLastChar(c);
			break;
		}
		break;
	case '*':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncEAsterisk;
			break;
		default:
			t->punctuator.c = puncAsterisk;
			s->pushLastChar(c);
			break;
		}
		break;
	case '/':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncESlash;
			break;
		default:
			t->punctuator.c = puncSlash;
			s->pushLastChar(c);
			break;
		}
		break;
	case '=':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncDEqual;
			break;
		default:
			t->punctuator.c = puncEqual;
			s->pushLastChar(c);
			break;
		}
		break;
	case '%':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncEPercent;
			break;
		default:
			t->punctuator.c = puncPercent;
			s->pushLastChar(c);
			break;
		}
		break;
	case '&':
		c = s->getNextChar();
		switch(c) {
		case '&':
			t->punctuator.c = puncDAmp;
			break;
		case '=':
			t->punctuator.c = puncEAmp;
			break;
		default:
			t->punctuator.c = puncAmp;
			s->pushLastChar(c);
			break;
		}
		break;
	case '|':
		c = s->getNextChar();
		switch(c) {
		case '|':
			t->punctuator.c = puncDBar;
			break;
		case '=':
			t->punctuator.c = puncEBar;
			break;
		default:
			t->punctuator.c = puncBar;
			s->pushLastChar(c);
			break;
		}
		break;
	case '<':
		c = s->getNextChar();
		switch(c) {
		case '<':
			c = s->getNextChar();
			switch(c) {
			case '=':
				t->punctuator.c = puncDELT;
				break;
			default:
				t->punctuator.c = puncDLT;
				s->pushLastChar(c);
				break;
			}
			break;
		case '=':
			t->punctuator.c = puncELT;
			break;
		default:
			t->punctuator.c = puncLT;
			s->pushLastChar(c);
			break;
		}
		break;
	case '>':
		c = s->getNextChar();
		switch(c) {
		case '>':
			c = s->getNextChar();
			switch(c) {
			case '=':
				t->punctuator.c = puncDEGT;
				break;
			default:
				t->punctuator.c = puncDGT;
				s->pushLastChar(c);
				break;
			}
			break;
		case '=':
			t->punctuator.c = puncEGT;
			break;
		default:
			t->punctuator.c = puncGT;
			s->pushLastChar(c);
			break;
		}
		break;
	case '!':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncEExcl;
			break;
		default:
			t->punctuator.c = puncExcl;
			s->pushLastChar(c);
			break;
		}
		break;
	case '^':
		c = s->getNextChar();
		switch(c) {
		case '=':
			t->punctuator.c = puncECaret;
			break;
		default:
			t->punctuator.c = puncCaret;
			s->pushLastChar(c);
			break;
		}
		break;
	default:
		t->punctuator.c = puncError;
		break;
	}
	
	t->type = tokenPunctuator;
	return true;
}

// Get next token in input stream without being able to push it back
void __getNextToken(Token *t, Stream *s) {
	int c = s->getNextChar();

	skipWhitespace(s, &c);
	
	if(c == EOF) { 
		t->type = tokenEOF;
		return;
	}

	if(isTokenStringLiteral(t, s, c)) { return; }
	
	if(isTokenIdentifier(t, s, c)) { return; }

	if(isTokenIntegerConstant(t, s, c)) { return; }

	if(isTokenPunctuator(t, s, c)) { return; }
}

void makeTokenStream(TokenStream *ts) {
	ts->t.type = tokenError;
}

void getNextToken(Token *t, TokenStream *ts) {
	if(ts->t.type != tokenError) {
		*t = ts->t;
		ts->t.type = tokenError;
		return;
	}

	__getNextToken(t, &ts->s);
}

void pushBackToken(TokenStream *ts, Token *t) {
	ts->t = *t;
}
