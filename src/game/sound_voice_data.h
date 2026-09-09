#ifndef MEMORIES_DECOMP_SOUND_VOICE_DATA_H
#define MEMORIES_DECOMP_SOUND_VOICE_DATA_H

#include "../types.h"

/* The voice data unit's entry points.
 *
 * func_80048A28 takes three plain words. Its callers had narrowed them to
 * u16/u8/s8, which reads as a description of the values rather than of the
 * function: the body decides what each word means by masking it, testing
 * arg0 against 0x8000 and against SD_VOICE_LOOKUP_CODE_MASK before using the
 * low bits, so the narrowing belongs at the call site and not in the type.
 *
 * func_80048D08's second argument is a word pointer, not a word. */
void func_80048A28(s32 arg0, s32 arg1, s32 arg2);
void func_80048C0C(u16 value, u8 enabled);
void func_80048C70(u32 *dst, u32 *src);
void func_80048D08(s32 side, u32 *src);
void func_80048F14(void);

#endif
