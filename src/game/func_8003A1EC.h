#ifndef MEMORIES_DECOMP_FUNC_8003A1EC_H
#define MEMORIES_DECOMP_FUNC_8003A1EC_H

#include "../types.h"
#include "display_object.h"
#include "menu_record.h"

/* Builds up to three display objects and writes them back through `out` as
 * out[0], out[1] and out[2], storing NULL in the slots it does not fill. That
 * is why the second parameter is DisplayObject **. The backing MenuRecord
 * grid has mixed consumers, so callers cast each three-word row to this
 * output view. func_8003AD6C passes rows 0 through 3, and each call fills its
 * own three-slot group. Returns 1 when it built anything and 0 when
 * DisplayEffect_HasResourceEntry rejected the first index; every caller
 * discards that result.
 *
 * The first parameter is the record itself. menu_record.h already names the
 * three fields this reads -- field_3C selects which of the two buffer bases
 * and which layer constants to use, and field_34/field_36 are the x/y pair
 * that header documents as "the pair func_8003A990 and func_8003AAE4 hand to
 * the position helpers". They are u16 there and read signed here, so the two
 * reads keep an explicit `*(s16 *)&` at the use.
 *
 * func_8003A560 in display_effect_resource_setup.c holds the same memory as a
 * DisplayEffectState instead and says so with its cast; menu_record.h records
 * that the two views name these same bytes. */
s32 func_8003A1EC(MenuRecord *a, DisplayObject **out, s32 c);

#endif
