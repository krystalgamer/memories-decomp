#ifndef MEMORIES_DECOMP_FUNC_8003A1EC_H
#define MEMORIES_DECOMP_FUNC_8003A1EC_H

#include "../types.h"

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is u8 ** and not the u8 * three of the callers
 * declared: func_8003AD6C passes p, p + 0xC, p + 0x18 and p + 0x24, and 0xC
 * is exactly three 4-byte pointers, so each call fills its own three-slot
 * group. Returns 1 when it built anything and 0 when func_8003A198 rejected
 * the first index; every caller discards that result. */
s32 func_8003A1EC(u8 *a, u8 **out, s32 c);

#endif
