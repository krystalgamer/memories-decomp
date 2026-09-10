#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H

#include "../types.h"
#include "duel_effect.h"

/* DuelEffect_HasActiveEntry reports whether a channel still has an entry in
 * flight, and func_8003741C is the D_80090E64 state that waits for that to
 * clear.
 *
 * func_800373C8 is the third function this unit defines. It was held back
 * until its narrowing parameters could be measured: it is defined
 * `(DuelEffectChannel *, u8, u8)` while both callers declared
 * `(DuelEffectChannel *, s32, s32)`, and a narrowing parameter can be load
 * bearing at the call site. Measured -- every call passes a small literal
 * (3/0, 2/0 and 0/0), so the argument is materialised by the same `li`
 * either way and the executable is unchanged. The declaration below is the
 * definition's own. */
s32 DuelEffect_HasActiveEntry(DuelEffectChannel *channel);
void func_8003741C(DuelEffectChannel *object);

/* Starting from the record's entry range, walks up to range_count_5E entries;
 * for each with flags_11 & 0x80 set, writes a1 to field_13 and a2 to
 * field_15, stopping at the first entry with that flag clear. */
void func_800373C8(DuelEffectChannel *channel, u8 a1, u8 a2);

#endif
