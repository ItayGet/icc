#include "lex.h"
#include "token.h"

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define MIN_IDEN_SIZE 16 * sizeof(char)

void syntaxError(const char *expected, const char *got, int line) {
	fprintf(stderr, "Syntax Error: expected %s, but got '%s' at line %d\n", expected, got, line);
	exit(1);
}

Token getNextToken(Stream *s) {
	int c = s->getNextChar();

	// Whether last iteration skipped characters
	bool success;

	// Skip multiple comments and whitespace
	do {
		success = false;
		
		// Skip whitespace
		while(true) {
			switch(c) {
			case ' ':
			case '\t':
			case '\n':
				success = true;
				c = s->getNextChar();
				continue;
			case EOF:;
				Token t;
				t.type = tokenEOF;
				
				return t;
			}
			break;
		}

		// Skip comments
		if(c == '/') {
			c = s->getNextChar();
			switch(c) {
			case '/':
				success = true;
				while((c = s->getNextChar()) != '\n') {
					if(c == EOF) { syntaxError("end of line", "EOF", 0); }
				}
				break;
			case '*':
				success = true;
				while(true) {
					c = s->getNextChar();
					if(c == EOF) {
						syntaxError("*/", "EOF", 0);
					}
					if(c == '*') {
						c = s->getNextChar();
						if(c != '/') {
							s->pushLastChar(c);
						}
						break;
					}
				}
				break;
			default:;
				char got[] = { c, 0 };
				syntaxError("/", got, 0);
			}
			c = s->getNextChar();
		}
	} while(success);

	// Identifier
	if(isalpha(c)) {
		size_t name_len = MIN_IDEN_SIZE;
		char* name = malloc(MIN_IDEN_SIZE);

		// Index into name
		int i = 0;
		do {
			// Handle realloc
			if(i == name_len - 1) {
				name_len += MIN_IDEN_SIZE;
				name = realloc(name, name_len);
			}

			name[i] = c;
			i++;

			c = s->getNextChar();
		} while(isalpha(c) || c == '_');
		s->pushLastChar(c);

		name[i] = 0;
		
		Token t;
		t.type = tokenIdentifier;
		t.identifier.name = name;

		return t;
	}
}
