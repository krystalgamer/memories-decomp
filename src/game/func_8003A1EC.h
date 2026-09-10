#ifndef MEMORIES_DECOMP_FUNC_8003A1EC_H
#define MEMORIES_DECOMP_FUNC_8003A1EC_H

#include "../types.h"
#include "menu_record.h"

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is u8 ** and not the u8 * three of the callers
 * declared: func_8003AD6C passes p, p + 0xC, p + 0x18 and p + 0x24, and 0xC
 * is exactly three 4-byte pointers, so each call fills its own three-slot
 * group. Returns 1 when it built anything and 0 when func_8003A198 rejected
 * the first index; every caller discards that result.
 *
 * The first parameter is the record itself. menu_record.h already names the
 * three fields this reads -- field_3C selects which of the two buffer bases
 * and which layer constants to use, and field_34/field_36 are the x/y pair
 * that header documents as "the pair func_8003A990 and func_8003AAE4 hand to
 * the position helpers". They are u16 there and read signed here, so the two
 * reads keep an explicit `*(s16 *)&` at the use.
 *
 * func_8003A560.c holds the same memory as a DisplayEffectState instead and
 * now says so with its cast; menu_record.h records that the two views name
 * these same bytes. */
s32 func_8003A1EC(MenuRecord *a, u8 **out, s32 c);

#endif
