#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H

#include "../types.h"

/* Two steps of the same field-wide effect machinery, both driven once per 16
 * frames and both walking the acting side's grid. func_80025F3C sweeps the
 * third grid row, staggering a type-8 effect object per slot and clearing a
 * negative stat modifier where it finds one; func_800260D0 is its sibling.
 * Both end their sweep by clearing D_8009B220. */
void func_80025F3C(void);
void func_800260D0(void);

#endif
