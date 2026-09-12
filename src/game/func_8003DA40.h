#ifndef MEMORIES_DECOMP_FUNC_8003DA40_H
#define MEMORIES_DECOMP_FUNC_8003DA40_H

#include "../types.h"
#include "menu_record.h"

/* D_80090F68 display effect step: opens the dialog's text channel. On its first
 * frame -- gated by DUEL_EFFECT_STATE_FLAG_INITIALIZED in D_8009B3C1 -- it
 * builds the display object and the channel through DuelEffect_CreateChannel,
 * then each frame steps the object and positions the channel over it. */
void func_8003DA40(MenuRecord *record);

#endif
