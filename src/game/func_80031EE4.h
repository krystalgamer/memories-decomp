#ifndef MEMORIES_DECOMP_FUNC_80031EE4_H
#define MEMORIES_DECOMP_FUNC_80031EE4_H

#include "../types.h"

/* Returns one copy of card `index` to the chest: bumps the owned count at
 * `base` + index + 0x5D97 and the total at +0x5A9C.
 *
 * The two arms differ in what else they touch. From zero owned, the card was
 * not in the chest list at all, so the entry whose id matches is marked
 * visible and the list is re-sorted; from a non-zero count below
 * CARD_CHEST_QUANTITY_MAX only the two numbers move. At the maximum it does
 * nothing, not even the total. */
void func_80031EE4(u8 *base, s32 index);

#endif
