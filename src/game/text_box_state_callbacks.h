#ifndef MEMORIES_DECOMP_TEXT_BOX_STATE_CALLBACKS_H
#define MEMORIES_DECOMP_TEXT_BOX_STATE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

typedef void (*TextBoxStateCallback)(DuelEffectChannel *);

extern TextBoxStateCallback D_80090E64[];

#endif
