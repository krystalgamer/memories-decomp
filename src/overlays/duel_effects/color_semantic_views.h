#ifndef MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H

#include "../../types.h"
#include "../../psyq/libgte.h"

extern s32 DuelEffect_IsColorBlack(const CVECTOR *color);
extern s32 DuelEffect_HasNonzeroColor(const CVECTOR *color);
extern void DuelEffect_DarkenColor(CVECTOR *color, u16 step);
extern s32 DuelEffect_StepColorToTarget(CVECTOR *color, u8 red, u8 green, u8 blue, u16 step);
extern void DuelEffect_SetFogBaseColor(u16 high, u16 middle, u16 low);
extern void DuelEffect_DrawFadingFullscreenColor(CVECTOR *color, u16 step);
extern void DuelEffect_DrawFullscreenGradient(CVECTOR *color);
extern void DuelEffect_DrawFullscreenColor(CVECTOR *color);

extern void DuelEffect_SetGrayColor(CVECTOR *color, u8 value);
extern void DuelEffect_SetRgbColor(CVECTOR *color, u8 red, u8 green, u8 blue);

#endif
