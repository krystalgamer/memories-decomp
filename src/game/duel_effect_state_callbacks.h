#ifndef MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

/* Eight entries of the D_80090E64 text-box state table, defined in one unit
 * because they share the same gating: each takes the state record, tests or
 * sets a bit of its 0x51 byte, and advances the state from there.
 *
 * They are reached only through the table, which indexes on the low five bits
 * of that same 0x51 byte -- so the byte is both the dispatch key and the state
 * each of these updates. */
void func_800378D8(DuelEffectChannel *object);
void func_80037914(DuelEffectChannel *object);
void func_80037950(DuelEffectChannel *object);
void func_8003798C(DuelEffectChannel *object);
void func_800379C4(DuelEffectChannel *object);
void func_800379F8(DuelEffectChannel *object);
void func_80037A58(DuelEffectChannel *object);
void func_80037B40(DuelEffectChannel *object);

#endif
