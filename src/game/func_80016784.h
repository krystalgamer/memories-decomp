#ifndef MEMORIES_DECOMP_FUNC_80016784_H
#define MEMORIES_DECOMP_FUNC_80016784_H

#include "../types.h"

/* Draws one record's glyph row.
 *
 * The second argument is passed straight through to func_80042188, which
 * takes it as a word in every declaration of it in the tree. One caller
 * hands it an element of the void * array D_800E9D98, so a pointer travels
 * through this word-sized channel; that conversion belongs at the call site
 * rather than in a declaration claiming the parameter is a pointer. */
void func_80016784(u8 *arg0, s32 arg1, s32 arg2, s32 arg3);

#endif
