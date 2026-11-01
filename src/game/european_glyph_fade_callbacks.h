#ifndef MEMORIES_DECOMP_EUROPEAN_GLYPH_FADE_CALLBACKS_H
#define MEMORIES_DECOMP_EUROPEAN_GLYPH_FADE_CALLBACKS_H

#include "../types.h"

#ifdef VERSION_EUROPE
#include "duel_effect.h"

void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context);
void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context);
void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context);
void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record);
#endif

#endif
