#include "lex.h"
#include "exprparser.h"

#include <stdio.h>

char progStr[] = "a || *b <<= + c";

#define size(arr) sizeof(arr)/sizeof(arr[0])

int i = 0;
int getNextChar() { if(i < size(progStr) - 1) { return progStr[i++]; } return EOF; }
void pushLastChar(int c) { progStr[--i] = c; }

int main() {
	
	Stream s = { getNextChar, pushLastChar };
	TokenStream ts;
	makeTokenStream(&ts);
	ts.s = s;

	ExprAst *expr = parseBinaryExpression(&ts, precAssignment);
}
