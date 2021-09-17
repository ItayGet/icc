#include "lex.h"
#include "exprparser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Temporary system: 
// arg "-" says to read from stdin,
// arg "-f <file>" reads from file,
// arg "-a <prog>" reads from args
int main(int argc, char *argv[]) {
	Stream s;

	if(argc == 2) {
		if(!strcmp(argv[1], "-")) {
			s.type = StreamFile;
			s.file.file = stdin;
		}
	}
	else if(argc == 3) {
		if(!strcmp(argv[1], "-f")) {
			s.type = StreamFile;
			s.file.file = fopen(argv[2], "r");
		} else if(!strcmp(argv[1], "-a")) {
			s.type = StreamArray;
			s.array.array = argv[2];
			s.array.pointer = argv[2];
		} else {
			puts("Args not properly formatted");
			exit(1);
		}
	} else {
		puts("Args not properly formatted");
		exit(1);
	}
		
	TokenStream ts;
	makeTokenStream(&ts);
	ts.s = s;

	ExprAst *expr = parseExpression(&ts);
}
