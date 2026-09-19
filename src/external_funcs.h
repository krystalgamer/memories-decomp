#ifndef MEMORIES_DECOMP_EXTERNAL_FUNCS_H
#define MEMORIES_DECOMP_EXTERNAL_FUNCS_H

#include "types.h"
#include "game/duel_effect_request.h"

/* Address-qualified second copies and caller-side views of external code. */
extern void SD_ConfigureTransferPhase_callback(void)
    asm("SD_ConfigureTransferPhase");
void func_801462B0(s16, s16, s32, DuelEffectRequest *);
void func_8016AA6C(void);
void func_801680F4(void);
s32 func_80168160(s32);

#endif
