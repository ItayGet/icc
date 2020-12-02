#include "lex.h"
#include "token.h"

#include <stdio.h>
#include <stdbool.h>

void syntaxError(const char *expected, const char *got, int line) {
	fprintf(stderr, "Syntax Error: expected %s, but got '%s' at line %d", expected, got, line);
	exit(1);
}

Token getNextToken(Stream *s) {
	int c = s.getNextChar();

	// Skip multiple comments and whitespace
	bool success;
	do {
		success = false;
		
		while(true) {
			c = getNextChar();
			switch(c) {
			case ' ':
			case '\t':
			case '\n':
				success = true;
				continue;
			case EOF:
				//return EOF token
			}
			pushLastChar(c);
			break;
		}

		// Skip comments
		if(c == '/') {
			c = s.getNextChar();
			switch(c) {
			case '/':
				success = true;
				while((c = s.getNextChar()) != '\n') {
					if(c == EOF) { syntaxError("end of line" "EOF", 0); }
				}
				break;
			case '*':
				success = true;
				while(true) {
					c = s.getNextChar();
					if(c == EOF) {
						syntaxError("*/", "EOF", 0);
					}
					if(c == '*') {
						c = s.getNextChar();
						if(c != '/') {
							pushLastChar(c);
						}
						break;
					}
				}
				break;
			default:
				char got[] = { c, 0 };
				syntaxError("/", got, 0);
			}
		}
	} while(success);

}
