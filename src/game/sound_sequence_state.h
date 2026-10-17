#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H

#include "../types.h"

/* The two BGM fade setters. Both store a signed per-tick step in field_1582
 * and a target level in field_1584; SD_UpdateFades moves field_1580 by the
 * step until it reaches the target. Their bodies are identical -- retail has
 * two symbols for the same two stores. SD_BGMFade reaches SD_StartBgmFade;
 * func_800490F0 is reached only from func_80047458, which has no caller. */
void func_800490F0(s16 value, u8 flag);
void SD_StartBgmFade(s16 step, u8 target);

/* The state query reads only g_SDValue. func_800379C4 still passes its effect
 * object in $a0, matching the retail caller sequence. */
#ifdef SD_IS_BGM_FADE_ACTIVE_IGNORES_OBJECT
s32 SD_IsBgmFadeActive(void *object);
#else
s32 SD_IsBgmFadeActive(void);
#endif

#endif
