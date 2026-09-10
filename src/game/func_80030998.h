#ifndef MEMORIES_DECOMP_FUNC_80030998_H
#define MEMORIES_DECOMP_FUNC_80030998_H

#include "../types.h"

/* Entry 2 of the frontend step table D_80090D84 (frontend_step_tables.c).
 * The unit implements it as a top-level asm block, so this prototype is what
 * the compiler sees on both sides; the block's own .globl resolves the table
 * entry. */
void func_80030998(void);

#endif
