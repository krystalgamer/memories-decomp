#ifndef MEMORIES_DECOMP_DUEL_EFFECT_CREATE_CHANNEL_H
#define MEMORIES_DECOMP_DUEL_EFFECT_CREATE_CHANNEL_H

#include "../types.h"
#include "duel_effect.h"

/* Opens a text-box channel for a dialog: clears the choice selection, builds
 * the box through TextBox_Create at a fixed 0x20,0x50 / 0x100x0x40 rectangle,
 * and hands the channel back.
 *
 * `value`'s low fifteen bits are the string id. Bit 15 is a request to run
 * func_80039A14 on the new channel -- but only when `set_flags` is zero;
 * a non-zero `set_flags` instead sets 0x1008 in the channel's flags_34 and
 * the bit is ignored. The two arms are exclusive, which the two callers rely
 * on: one passes 0x80D4 with set_flags zero, the other 0xD0.
 *
 * The return is the channel, and it is worth having typed: dialog_transition.c
 * already declared it DuelEffectChannel * while func_8003DA40.c held `u8 *`
 * and reads the same record at raw +0x30 and +0x34. It casts. */
DuelEffectChannel *DuelEffect_CreateChannel(s32 value, s32 set_flags);

#endif
