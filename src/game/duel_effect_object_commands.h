#ifndef MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_COMMANDS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_OBJECT_COMMANDS_H

#include "../types.h"
#include "duel_effect.h"

void func_800389C4(DuelEffectChannel *value);
/* Secondary commands 0x17/0x18: index a u16 target table with independent
 * shared selectors, then patch the same post-read stream pointer's low half.
 * Producer-specific meanings and the regional high mask remain separate. */
void Text_JumpBySelectorA(DuelEffectChannel *object);
void Text_JumpBySelectorB(DuelEffectChannel *object);
/* Secondary text command 0x19: consumes a duelist id and marks both its
 * campaign defeat and Free Duel unlock flags. Zero is a no-op. */
void Text_UnlockDuelist(DuelEffectChannel *object);
/* Secondary text command 0x1A: releases the active choice object and returns
 * the channel to state 2, yielding the current text-building pass. */
void Text_CloseChoice(DuelEffectChannel *object);

#endif
