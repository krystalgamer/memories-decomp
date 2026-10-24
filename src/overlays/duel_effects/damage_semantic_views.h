#ifndef MEMORIES_DECOMP_DUEL_DAMAGE_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_DAMAGE_SEMANTIC_VIEWS_H

#include "effect_6.h"
#include "effect_7.h"

extern void DuelEffect_RunDirectDamage(void *buffer, s32 phase) __asm__("func_80154B30");
extern void DuelEffect_RunReflectedDirectDamage(void *buffer, s32 phase) __asm__("func_801587D8");

extern DuelEffect6Config gDuelEffect_DirectDamageConfigs[6] __asm__("D_8015B30C");
extern DuelEffect7Config gDuelEffect_ReflectedDirectDamageConfigs[5] __asm__("D_8015B5B8");
extern VECTOR gDuelEffect_DirectDamageInitialScale __asm__("D_801461F8");
extern VECTOR gDuelEffect_ReflectedDirectDamageInitialScale __asm__("D_80146228");

extern void func_80154B30(void *buffer, s32 phase);
extern void func_801587D8(void *buffer, s32 phase);

#endif
