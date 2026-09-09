#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ENTRY_CONTROL_H

#include "../types.h"
#include "duel_effect.h"

/* DuelEffect_HasActiveEntry reports whether a channel still has an entry in
 * flight, and func_8003741C is the D_80090E64 state that waits for that to
 * clear.
 *
 * func_800373C8, the third function this unit defines, is deliberately absent.
 * It is defined `(DuelEffectChannel *, u8, u8)` and both its callers declare
 * `(DuelEffectChannel *, s32, s32)`; a narrowing parameter can be load bearing
 * at the call site, so it needs its own measurement -- and one of the two
 * callers has other work in flight. */
s32 DuelEffect_HasActiveEntry(DuelEffectChannel *channel);
void func_8003741C(u8 *object);

#endif
