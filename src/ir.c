#include "ir.h"

#include <stdlib.h>

void cleanIrProg(IrProg *prog) {
	if(!prog) { return; }
	
	// TODO: Clean token
	
	cleanIrProg(prog->next);
	free(prog);
}

void backpatchBackpatchList(BackpatchListNode *bln, IrProg **label) {
	if(!bln) { return; }

	// FIXME: What if action does not use label?
	
	(*bln->val)->val.label = label;
	backpatchBackpatchList(bln->next, label);
	free(bln);
}
