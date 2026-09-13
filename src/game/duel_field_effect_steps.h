#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H

#include "../types.h"

/* func_80025F3C creates a type-0x15 controller, waits for its count, then
 * sweeps slots 5..9 and installs func_80025B28 on cards carrying both high
 * modifier bits before completing the opposite-side state. func_800260D0 is
 * the sixteen-frame sibling: it staggers type-8 objects across slots 10..14
 * and clears negative stat modifiers. Both finish by clearing D_8009B220. */
void func_80025F3C(void);
void func_800260D0(void);

#endif
