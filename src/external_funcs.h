#ifndef MEMORIES_DECOMP_EXTERNAL_FUNCS_H
#define MEMORIES_DECOMP_EXTERNAL_FUNCS_H

#include "types.h"
#include "psyq/libcd.h"
#include "game/duel_effect_request.h"

/* Address-qualified second copies and caller-side views of external code. */
#ifdef CD_POS_TO_INT_UNSPECIFIED_VIEW
s32 CdPosToInt_8007E710();
#elif defined(CD_POS_TO_INT_LOCATION_VIEW)
s32 CdPosToInt_8007E710(const CdlLOC *);
#else
s32 CdPosToInt_8007E710(s32);
#endif
#ifdef CD_INT_TO_POS_THREE_ARGUMENT_VIEW
void CdIntToPos_8007E600(s32, void *, s32);
#else
void CdIntToPos_8007E600(s32, void *);
#endif

extern void func_80014B30_callback(void) asm("func_80014B30");
void func_800862C0(void *);
void func_801462B0(s16, s16, s32, DuelEffectRequest *);
void func_8016AA6C(void);
void func_801680F4(void);
s32 func_80168160(s32);

#endif
