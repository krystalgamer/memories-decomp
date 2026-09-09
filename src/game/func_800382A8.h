#ifndef MEMORIES_DECOMP_FUNC_800382A8_H
#define MEMORIES_DECOMP_FUNC_800382A8_H

#include "../types.h"

/* D_80090EAC entry: sets the object's glyph cell size from a one-byte operand --
 * 8 by 8 for 1, 8 by 12 for 2, unchanged otherwise -- and mirrors case 1 into
 * bit 0x100 of the object's 0x34 flags, which it clears first either way. The
 * body is register-pinned throughout. */
void func_800382A8(u8 *object);

#endif
