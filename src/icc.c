#include "parser.h"
#include "token-stream.h"

#include <stdlib.h>
#include <stdio.h>

char a[] = 
"a = 5, b = a + 2";

const char *b[] =
{
	#define ACTION(NAME, STR) STR,
	#include "iraction-definitions.h"
};

void stringifyArg(IrArg *arg, char* s) {
	switch(arg->type) {
	case argConst:
		sprintf(s, "%d", arg->t->integer.value);
		break;
	case argInstr:
		sprintf(s, "t");
		break;
	case argSymbol:
		sprintf(s, "%s", arg->s->variable.name);
		break;
	}
}

void printInstr(IrInstr *instr) {
	char* argA = malloc(sizeof(char) * 1024);
	stringifyArg(&instr->a, argA);

	char* argB;
	if(instr->action < actionSeparator) {
		argB = malloc(sizeof(char) * 1024);
		stringifyArg(&instr->b, argB);
	}

	printf(b[instr->action], argA, argB);
	putchar('\n');
	free(argA);
	if(argA != argB) { free(argB); }
}

#define size sizeof(a)/sizeof(a[0])
int i = 0;
int getNextChar() { if(i < size - 1) { return a[i++]; } return EOF; }
void pushLastChar(int c) { a[--i] = c; }

int main() {
	Stream s = { getNextChar, pushLastChar };
	TokenStream ts = { s };

	IrProg *prog = malloc(sizeof(IrProg));
	IrProg *lastNode = prog;

	IrProg *retProg = malloc(sizeof(IrProg));

	parseExpression(&retProg->val, &lastNode, &ts);

	lastNode->next = retProg;
	lastNode = prog;
	prog = prog->next;
	free(lastNode);

	while(prog) {
		printInstr(&prog->val);

		prog = prog->next;
	}
}
