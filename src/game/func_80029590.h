#ifndef MEMORIES_DECOMP_FUNC_80029590_H
#define MEMORIES_DECOMP_FUNC_80029590_H

#include "../types.h"

/* Clears every pending effect-resource request in one pass -- the bulk form of
 * func_80029574, and what a screen calls when it is torn down rather than
 * stepped. */
void func_80029590(void);

#endif
