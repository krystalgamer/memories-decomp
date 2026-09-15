#ifndef MEMORIES_DECOMP_FUNC_80040814_H
#define MEMORIES_DECOMP_FUNC_80040814_H

#include "../types.h"

/* The gDisplayObject_ListRenderers entry for the single-headed list at D_800EFE3C -- the one
 * list whose head is a scalar rather than an entry of D_800EFE38. Runs each
 * object's callback and renders the renderable ones. */
void func_80040814(void);

#endif
