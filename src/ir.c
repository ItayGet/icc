#include "ir.h"

#include <stdlib.h>

IrProg *appendIrProgNode(IrProg *prog) {
	IrProg *new = malloc(sizeof(IrProg));
	prog->next = new;

	return new;
}
