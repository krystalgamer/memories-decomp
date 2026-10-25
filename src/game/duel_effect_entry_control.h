#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H

#include "../types.h"
#include "duel_effect.h"

typedef struct {
    u8 pad_00[0x5C];
    u8 range_start_5C;
    u8 range_count_5D;
} JapaneseDuelEffectEntryRange;

/* DuelEffect_HasActiveEntry checks the active prefix for a nonzero control
 * byte. The separate Text_CompletePageAdvance state requests clearing, then
 * tests the first configured entry's active flag on subsequent calls.
 *
 * DuelEffect_SetEntryControlBytes is the third function this unit defines. It was held back
 * until its narrowing parameters could be measured: it is defined
 * `(DuelEffectChannel *, u8, u8)` while both callers declared
 * `(DuelEffectChannel *, s32, s32)`, and a narrowing parameter can be load
 * bearing at the call site. Measured resident calls pass small literals
 * (3/0, 2/0 and 0/0), so the argument is materialised by the same `li`
 * either way and the executable is unchanged. The declaration below is the
 * definition's own. The shared Spanish intro also passes raw control bytes
 * 5, 6, 8 and 9 with render mode zero; those are not a page-only enum. */
#ifdef VERSION_EUROPE
extern s32 DuelEffect_HasActiveEntry(DuelEffectChannel *channel) asm("func_800370E0");
#else
s32 DuelEffect_HasActiveEntry(DuelEffectChannel *channel);
#endif
void Text_CompletePageAdvance(DuelEffectChannel *object);

/* Starting from the record's entry range, walks up to its regional count;
 * for each with flags_11 & 0x80 set, writes a1 to field_13 and a2 to
 * field_15, stopping at the first entry with that flag clear. */
void DuelEffect_SetEntryControlBytes(DuelEffectChannel *channel, u8 a1, u8 a2);

#endif
