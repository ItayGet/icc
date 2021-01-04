#include "tokenstream.h"

#include <stddef.h>

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
