#include "lex.h"
#include "exprparser.h"

#include <stdio.h>

char progStr[] = "(int) a ++ + (a?b:c)";

int main() {
	
	Stream s;
	s.type = StreamArray;
	s.array.array = progStr;
	s.array.pointer = progStr;
		
	TokenStream ts;
	makeTokenStream(&ts);
	ts.s = s;

	ExprAst *expr = parseExpression(&ts);
}
