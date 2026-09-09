#ifndef MEMORIES_DECOMP_FUNC_8002E128_H
#define MEMORIES_DECOMP_FUNC_8002E128_H

#include "../types.h"

/* Rebuilds the display objects for one scene-script image record. `value` is
 * the packed image id, or negative to reuse the one already stored at the
 * record's +0x3C; either way that halfword ends up holding what was built.
 *
 * Ids from 0x200 up are the D_80090C00 table and get a full-screen object plus
 * up to two overlays positioned from the table row; below that the object
 * comes from func_8002E060 and, from 0x100 up, D_80090BA8's flag byte decides
 * which of the two overlays exist. Under 0x100 the record's two overlay words
 * are just cleared.
 *
 * Both callers spelled the record `void *`; it is a u8 * here because that is
 * what the definition walks. */
void func_8002E128(u8 *record, s32 value);

#endif
