#ifndef MEMORIES_DECOMP_SOUND_VOICE_DATA_H
#define MEMORIES_DECOMP_SOUND_VOICE_DATA_H

#include "../types.h"

/* The voice data unit's entry points.
 *
 * SD_SESetVolumePan takes three plain words at the call site. Its definition,
 * with SD_FadeOutSoundEffectVoices among the sound-effect controls in
 * sound_effect_request.c, is old style with u16/u8/u16 parameters, which default
 * promotion keeps compatible with this prototype: the narrowing happens in
 * the callee, where GCC keeps each incoming register and its narrowed copy
 * apart exactly as retail does.
 *
 * SD_LoadSequenceBankPair's second argument is a word pointer, not a word. */
void SD_SESetVolumePan(s32 arg0, s32 arg1, s32 arg2);
void SD_FadeOutSoundEffectVoices(u16 id, u8 step);
void SD_CopySeqBlock(u32 *dst, u32 *src);
void SD_LoadSequenceBankPair(s32 side, u32 *src);
void SD_InitSecondaryRuntime(void);

#endif
