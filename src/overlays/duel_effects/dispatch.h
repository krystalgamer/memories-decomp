#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DISPATCH_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DISPATCH_H

#include "../../types.h"
#include "../../game/duel_effect_request.h"
#include "drawing_helpers.h"
#include "textured_quads.h"
#include "image_inputs.h"
#include "effect_17.h"

extern GsIMAGE D_8015A1E4[21];

void func_80146258(s32 effect, s32 phase, void *buffer, DuelEffectRequest *context);
void func_80146760(void *work, s32 phase);
void func_80147B18(void *work, s32 phase);
void func_801481A8(void *work, s32 phase);
void func_80149F90(void *work, s32 phase);
void func_8014A8E4(void *work, s32 phase);
void func_8014C8FC(void *work, s32 phase);
void func_8014D3E8(void *work, s32 phase);
void func_8014E3EC(void *work, s32 phase);
void func_8014FF40(void *work, s32 phase);
void func_801503F8(void *work, s32 phase, s16 variant);
void func_80150E00(void *work, s32 phase);
void func_80151558(void *work, s32 phase);
void func_80152048(void *work, s32 phase);
void func_80153200(void *work, s32 phase, s16 variant);
void func_80153ADC(void *work, s32 phase);
void func_80154084(void *work, s32 phase);
void func_80154688(void *work, s32 phase);
void func_80154B30(void *work, s32 phase);
void func_80157794(void *work, s32 phase);
void func_80157E10(void *work, s32 phase);
void func_801587D8(void *work, s32 phase);
void func_801593A8(void *work, s32 phase);
void func_80159AAC(void *work, s32 phase);

#endif
