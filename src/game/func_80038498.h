#ifndef MEMORIES_DECOMP_FUNC_80038498_H
#define MEMORIES_DECOMP_FUNC_80038498_H

#include "../types.h"

/* D_80090EAC entry: sets the object's colour slot at +0x54 from a one-byte
 * operand. With bit 7 set the low nibble indexes gText_abColorSlots instead of
 * being used directly, so a palette entry and a literal colour share one
 * opcode. */
void func_80038498(u8 *object);

#endif
