#ifndef MEMORIES_DECOMP_FUNC_80033998_H
#define MEMORIES_DECOMP_FUNC_80033998_H

#include "../types.h"

/* Reports whether the deck has a free slot.
 *
 * It walks DECK_SIZE entries of sixteen bytes each, starting 0x2D50 into the
 * block D_8009B2FC points at, and returns 1 as soon as it finds one whose
 * byte at +0x0D is zero. It returns 0 when every entry is occupied.
 *
 * What the byte at +0x0D means beyond empty-or-not is not established here,
 * and the name stays address-based for that reason: the scan is legible, the
 * field is not.
 *
 * func_800339D0.c is the only consumer, and its local extern already agreed
 * apart from spelling the return s32 where the definition writes int. */
int func_80033998(void);

#endif
