#include "symbol.h"

#include <stdlib.h>

void cleanSymbol(Symbol *s) {
	switch(s->type) {
	case symbolType:
		cleanType(s->typeS);
		break;
	case symbolFunction:
		free(s->function.name);
		break;
	case symbolVariable:
		cleanType(s->variable.type);
		break;
	}
	free(s);
}
