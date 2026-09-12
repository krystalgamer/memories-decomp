#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H

#include "../types.h"

/* Initializes a type-0x15 object for the side opposite D_8009B1D5, then
 * scans acting-side slots 5..9 and installs func_80025B28 on qualifying card
 * objects. Once the effect is ready, it stores 4 in the opposite side's
 * +0x19 counter and clears D_8009B220. */
void func_80025F3C(void);

/* Advances a 16-frame sweep through acting-side slots 10..14, spawning one
 * staggered type-8 object per slot. A swept occupied card with a negative
 * stat modifier has that modifier cleared; the step ends by clearing
 * D_8009B220. */
void func_800260D0(void);

#endif
