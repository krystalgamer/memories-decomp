#ifndef MEMORIES_DECOMP_FUNC_80029EC4_H
#define MEMORIES_DECOMP_FUNC_80029EC4_H

/* Draws the Library card-list grid and its cursor box. Never called from C:
 * func_8002BFCC (library_runtime.c) and func_8002ACA4 install its address in
 * D_800E9DB0[3], the per-frame draw callback slot. */
void func_80029EC4(void);

#endif
