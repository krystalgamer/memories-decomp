#ifndef MEMORIES_DECOMP_DUEL_EFFECT_UPDATE_OBJECT_LAYOUT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_UPDATE_OBJECT_LAYOUT_H

#include "duel_effect.h"

/* Repositions the six corner pairs of the channel's attached object so the
 * highlight sits on the currently selected choice. The entry's field_18
 * taken modulo ten selects one of three layouts: 1 is the 0x10-pitch list,
 * 2 and 0 are 0xC-pitch lists offset by two pixels either way. */
void DuelEffect_UpdateObjectLayout(DuelEffectChannel *p);

#endif
