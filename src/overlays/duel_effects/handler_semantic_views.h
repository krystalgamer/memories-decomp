#ifndef MEMORIES_DECOMP_DUEL_HANDLER_SEMANTIC_VIEWS_H
#define MEMORIES_DECOMP_DUEL_HANDLER_SEMANTIC_VIEWS_H

#include "../../types.h"

/* Live-traced effect handlers; native owners and labels are unchanged. */
extern void DuelEffect_RunFusion(void *work, s32 phase) __asm__("func_80157794");
extern void DuelEffect_RunAttackSlash(void *work, s32 phase, s16 damage) __asm__("func_80153200");
extern void DuelEffect_RunDestroyFlames(void *work, s32 phase) __asm__("func_80149F90");
extern void DuelEffect_RunTerrainChange(void *work, s32 phase) __asm__("func_8014C8FC");
extern void DuelEffect_RunGuardianStarBonus(void *work, s32 phase) __asm__("func_801481A8");
extern void DuelEffect_RunBoardDestruction(void *work, s32 phase) __asm__("func_80148BA4");

extern void func_80157794(void *work, s32 phase);
extern void func_80153200(void *work, s32 phase, s16 damage);
extern void func_80149F90(void *work, s32 phase);
extern void func_8014C8FC(void *work, s32 phase);
extern void func_801481A8(void *work, s32 phase);
extern void func_80148BA4(void *work, s32 phase);

#endif
