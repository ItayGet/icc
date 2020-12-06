#include "literal.h"

#include "stringbuilder.h"

char getStringChar(Stream *s, int c) {
	if(c != '\\') { return c; }

	// Escape characters
	c = s->getNextChar();

	switch(c) {
	case '0': return 0;
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n': return '\n';
	case 'r': return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	case '\\': return '\\';
	case '\'': return '\'';
	case '"': return '\"';
	case '?': return '\?';
	default: return c;
	}
}

bool isTokenStringLiteral(Token *t, Stream *s, int c) {	
	if(c != '"') { return false; }

	StringBuilder sb;
	makeStringBuilder(&sb, CAP_INC_SB);

	while((c = s->getNextChar()) != '"') {
		appendStringBuilderChar(&sb, getStringChar(s, c));
	}

	stringifyStringBuilderBuffer(&sb);

	t->type = tokenStringLiteral;
	t->stringLiteral.contents = sb.buffer;

	return true;

}


