#ifndef MEMORIES_DECOMP_FUNC_80031354_H
#define MEMORIES_DECOMP_FUNC_80031354_H

#include "../types.h"

/* Entry 6 of the frontend step table D_80090D84 (frontend_step_tables.c).
 * On entry it loads campaign scene package 0. Each frame it then runs
 * func_8003134C, and while D_8009B2DE is non-zero it passes D_800EB010 and
 * Rand_GetInterval(4) to func_8003B378. Once D_8009B2DE is zero it makes two display objects
 * renderable and clears the step byte. */
void func_80031354(void);

#endif
