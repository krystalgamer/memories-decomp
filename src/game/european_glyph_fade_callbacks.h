#ifndef MEMORIES_DECOMP_EUROPEAN_GLYPH_FADE_CALLBACKS_H
#define MEMORIES_DECOMP_EUROPEAN_GLYPH_FADE_CALLBACKS_H

#include "../types.h"

#ifdef VERSION_EUROPE
#include "duel_effect.h"
#include "func_80039B3C.h"

extern void func_80039D24(DuelEffectChannel *record, void *context);
extern void func_80039D84(DuelEffectChannel *record, void *context);
extern void func_80039DF8(DuelEffectChannel *record);
extern void func_80039E88(DuelEffectChannel *record);
extern void func_80039EA8(DuelEffectChannel *record);

extern void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context)
    __asm__("func_80039B3C");
extern void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context)
    __asm__("func_80039D24");
extern void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context)
    __asm__("func_80039D84");
extern void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record)
    __asm__("func_80039DF8");
extern void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record)
    __asm__("func_80039E88");
extern void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record)
    __asm__("func_80039EA8");
#endif

#endif
