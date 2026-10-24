#ifndef MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_COLOR_SEMANTIC_VIEWS_H

#include "../../types.h"

/* Keep semantic declarations separate from fingerprinted native headers. */
extern s32 DuelEffect_IsColorBlack(const u8 *color) __asm__("func_8014D378");
extern s32 DuelEffect_HasNonzeroColor(const u8 *color) __asm__("func_8014D3AC");
extern void DuelEffect_DarkenColor(u8 *color, u16 step) __asm__("func_80153F28");
extern s32 DuelEffect_StepColorToTarget(u8 *color, u8 red, u8 green, u8 blue, u16 step) __asm__("func_80153F98");
extern void DuelEffect_SetFogBaseColor(u16 high, u16 middle, u16 low) __asm__("func_8015405C");
extern void DuelEffect_DrawFadingFullscreenColor(u8 *color, u16 step) __asm__("func_801556F4");
extern void DuelEffect_DrawFullscreenGradient(u8 *color) __asm__("func_80155798");
extern void DuelEffect_DrawFullscreenColor(u8 *color) __asm__("func_80155864");

/* Track the existing native contracts without editing fingerprinted owners. */
extern s32 func_8014D378(const u8 *color);
extern s32 func_8014D3AC(const u8 *color);
extern void func_80153F28(u8 *color, u16 step);
extern s32 func_80153F98(u8 *color, u8 red, u8 green, u8 blue, u16 step);
extern void func_8015405C(u16 high, u16 middle, u16 low);
extern void func_801556F4(u8 *color, u16 step);
extern void func_80155798(u8 *color);
extern void func_80155864(u8 *color);

#endif
