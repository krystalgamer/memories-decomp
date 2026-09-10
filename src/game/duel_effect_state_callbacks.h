#ifndef MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

/* Fifteen entries of the D_80090E64 text-box state table, plus the choice
 * object constructor used by the first interaction state. Each callback takes
 * the state record, tests or sets its 0x51 byte, and advances from there.
 *
 * They are reached only through the table, which indexes on the low five bits
 * of that same 0x51 byte -- so the byte is both the dispatch key and the state
 * each of these updates. */
void func_800374A8(DuelEffectChannel *object);
void func_800375A4(DuelEffectChannel *object);
void func_8003767C(DuelEffectChannel *object);
void func_8003771C(DuelEffectChannel *object);
void func_800377AC(DuelEffectChannel *object);
void func_800377C8(DuelEffectChannel *object);
void func_8003787C(DuelEffectChannel *object);
void func_800378D8(DuelEffectChannel *object);
void func_80037914(DuelEffectChannel *object);
void func_80037950(DuelEffectChannel *object);
void func_8003798C(DuelEffectChannel *object);
void func_800379C4(DuelEffectChannel *object);
void func_800379F8(DuelEffectChannel *object);
void func_80037A58(DuelEffectChannel *object);
void func_80037B40(DuelEffectChannel *object);

/* The gate on the script state machine, armed by func_8003767C and
 * func_8003771C and spent by TextBox_BuildStep. */
extern u8 D_8009B357;

#endif
