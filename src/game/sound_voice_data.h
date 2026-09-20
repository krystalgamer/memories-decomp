#ifndef MEMORIES_DECOMP_SOUND_VOICE_DATA_H
#define MEMORIES_DECOMP_SOUND_VOICE_DATA_H

#include "../types.h"

/* The voice data unit's entry points.
 *
 * func_80048A28 takes three plain words at the call site. Its definition in
 * sound_voice_data.c is old style with u16/u8/u16 parameters, which default
 * promotion keeps compatible with this prototype: the narrowing happens in
 * the callee, where GCC keeps each incoming register and its narrowed copy
 * apart exactly as retail does.
 *
 * SD_LoadSequenceBankPair's second argument is a word pointer, not a word. */
void func_80048A28(s32 arg0, s32 arg1, s32 arg2);
void func_80048C0C(u16 value, u8 enabled);
void func_80048C70(u32 *dst, u32 *src);
void SD_LoadSequenceBankPair(s32 side, u32 *src);
void SD_InitSecondaryRuntime(void);

#endif
