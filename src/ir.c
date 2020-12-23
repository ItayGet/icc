#include "ir.h"

#include <stdlib.h>

void cleanIrProg(IrProg *prog) {
	if(!prog) { return; }
	
	// TODO: Clean token
	
	cleanIrProg(prog->next);
	free(prog);
}
