#ifndef MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H

#include "../../types.h"

extern s32 DuelEffect_IsColorBlack(const u8 *color);
extern s32 DuelEffect_HasNonzeroColor(const u8 *color);
extern void DuelEffect_DarkenColor(u8 *color, u16 step);
extern s32 DuelEffect_StepColorToTarget(u8 *color, u8 red, u8 green, u8 blue, u16 step);
extern void DuelEffect_SetFogBaseColor(u16 high, u16 middle, u16 low);
extern void DuelEffect_DrawFadingFullscreenColor(u8 *color, u16 step);
extern void DuelEffect_DrawFullscreenGradient(u8 *color);
extern void DuelEffect_DrawFullscreenColor(u8 *color);

extern void DuelEffect_SetGrayColor(u8 *color, u8 value);
extern void DuelEffect_SetRgbColor(u8 *color, u8 red, u8 green, u8 blue);

#endif
