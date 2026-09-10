#ifndef DISPLAY_OBJECT_FADE_H
#define DISPLAY_OBJECT_FADE_H

#include "../types.h"
#include "duel_effect.h"

#define DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE 0x40
#define DISPLAY_OBJECT_FADE_FLAG_INITIALIZED 0x80

s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *);

#endif
