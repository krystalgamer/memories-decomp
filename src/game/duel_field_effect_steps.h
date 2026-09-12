#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H

#include "../types.h"

/* Field-wide stat-restoration step driven once per 16 frames. It sweeps the
 * third grid row, staggering a type-8 effect object per slot and clearing a
 * negative stat modifier where it finds one. */
void func_800260D0(void);

#endif
