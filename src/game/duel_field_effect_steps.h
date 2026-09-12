#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_STEPS_H

#include "../types.h"

/* Two steps of the same field-wide effect machinery. func_80025F3C creates
 * the type-0x15 controller, then scans acting-side slots 5 through 9 and
 * activates func_80025B28 on qualifying card objects before clearing
 * D_8009B220 at completion; func_800260D0 is its sibling. */
void func_80025F3C(void);
void func_800260D0(void);

#endif
