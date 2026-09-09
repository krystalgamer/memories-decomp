#ifndef MEMORIES_DECOMP_BYTE_TRIPLET_HELPERS_H
#define MEMORIES_DECOMP_BYTE_TRIPLET_HELPERS_H

#include "../types.h"

/* Three helpers over groups of three consecutive bytes: set from three
 * values, copy, and take the per-byte mean of two groups.
 *
 * They are spelled as plain byte pointers rather than a triplet type because
 * that is all the bodies assume: each touches exactly indices 0, 1 and 2 of
 * whatever it is handed, and none of them reads a fourth byte or a stride.
 * The callers' buffers are not described here. */
void func_8006C2FC(u8 *output, s32 first, s32 second, s32 third);
void func_8006C30C(u8 *destination, const u8 *source);
void func_8006C330(u8 *out, u8 *a, u8 *b);

#endif
