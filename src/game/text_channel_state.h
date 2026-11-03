#ifndef MEMORIES_DECOMP_TEXT_CHANNEL_STATE_H
#define MEMORIES_DECOMP_TEXT_CHANNEL_STATE_H

#include "duel_effect.h"

extern DuelEffectChannel gText_Channel1;

/* Existing word view: channel 0's flags and string ID, not another object. */
#ifdef D_800EB12C_IN_DATA
extern u32 gText_dwChannel0State[];
#else
extern u32 gText_dwChannel0State;
#endif

void Text_DisableCompletionWait(DuelEffectChannel *value);

#endif
