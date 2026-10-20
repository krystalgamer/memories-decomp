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
void Text_BeginPageClear(DuelEffectChannel *object);
void Text_WaitForPageAdvance(DuelEffectChannel *object);
/* Shared signed-byte Square-hold countdown; not per-channel storage. */
extern s8 gText_PageAdvanceHoldCountdown;
void func_8003767C(DuelEffectChannel *object);
void func_8003771C(DuelEffectChannel *object);
void TextBox_WaitForScriptCompletion(DuelEffectChannel *object);
void func_800377C8(DuelEffectChannel *object);
void func_8003787C(DuelEffectChannel *object);
void func_800378D8(DuelEffectChannel *object);
void func_80037914(DuelEffectChannel *object);
void func_80037950(DuelEffectChannel *object);
void func_8003798C(DuelEffectChannel *object);
void func_800379C4(DuelEffectChannel *object);
/* State 15: initialization consumes a word and decrements in the same call. */
void Text_WaitForDelay(DuelEffectChannel *object);
/* State 16: shares that counter; optional jitter precedes completion restore. */
void Text_WaitWithViewportShake(DuelEffectChannel *object);
/* Shared delay counter and shake origin, not per-channel state. */
extern s16 gText_WaitCounter;
extern u16 gText_ShakeViewportOrigin[2];
void func_80037B40(DuelEffectChannel *object);

/* The gate on the script state machine, armed by func_8003767C and
 * func_8003771C and spent by TextBox_BuildStep. */
extern u8 D_8009B335;
extern u8 D_8009B357;

#endif
