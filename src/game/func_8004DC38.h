#ifndef MEMORIES_DECOMP_FUNC_8004DC38_H
#define MEMORIES_DECOMP_FUNC_8004DC38_H

#include "../types.h"

/* Seeks channel i of a model slot to position pos within sequence n.
 *
 * The first argument is a ModelSlot; all three callers hold one and two of
 * them said so in their own prototypes. It is declared as a byte pointer
 * because the definition cannot take the typed one: introducing the cast,
 * at any point in the body, changes the entry instruction at 0x8004DC38.
 * That function's own comments explain why -- the two channel offsets are
 * materialised up front specifically to move this pointer out of $a0. */
void func_8004DC38(u8 *slot, s32 i, s32 n, u32 pos);

#endif
