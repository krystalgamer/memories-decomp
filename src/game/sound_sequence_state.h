#ifndef MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H
#define MEMORIES_DECOMP_SOUND_SEQUENCE_STATE_H

#include "../types.h"

/* The two BGM fade setters. Both store a signed per-tick step in bgm_fade_step
 * and a target level in bgm_fade_target; SD_UpdateFades moves bgm_fade_level by the
 * step until it reaches the target. Their bodies are identical -- retail has
 * two symbols for the same two stores. SD_BGMFade reaches SD_StartBgmFade;
 * tent_SD_StartBgmFadeCopy is reached only from tent_SD_BGMFadeCopy, which
 * has no caller. */
void tent_SD_StartBgmFadeCopy(s16 value, u8 flag);
void SD_StartBgmFade(s16 step, u8 target);

/* The state query reads only g_SDValue. Text_WaitForBgmFade still passes its effect
 * object in $a0, matching the retail caller sequence. */
#ifdef SD_IS_BGM_FADE_ACTIVE_IGNORES_OBJECT
s32 SD_IsBgmFadeActive(void *object);
#else
s32 SD_IsBgmFadeActive(void);
#endif

#endif
