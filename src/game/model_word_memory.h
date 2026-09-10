#ifndef MEMORIES_DECOMP_MODEL_WORD_MEMORY_H
#define MEMORIES_DECOMP_MODEL_WORD_MEMORY_H

#include "../types.h"

/* The two word-at-a-time memory primitives. Both take `count` in WORDS, not
 * bytes: each loop body moves one s32 and advances by one element. Callers
 * working in bytes scale by four themselves -- func_80057AF4
 * (src/candidates/func_80057AF4.c) follows its
 * copy with `dst += count * 4`, and file_transfer_steps.c copies 0x40 words
 * and then reads the next record at +0x100. */
void func_8005B5FC(s32 *destination, s32 value, u32 count);
void func_8005B620(s32 *destination, const s32 *source, u32 count);

#endif
