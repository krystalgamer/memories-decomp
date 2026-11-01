#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DISPATCH_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "drawing_helpers.h"
#include "textured_quads.h"
#include "image_inputs.h"
#include "effect_17.h"

extern void DuelEffect_DispatchRequest(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context);

#endif
