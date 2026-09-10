#ifndef MEMORIES_DECOMP_FUNC_8002A788_H
#define MEMORIES_DECOMP_FUNC_8002A788_H

#include "../types.h"

/* State 1 of the library screen dispatcher, handed the same record the other
 * states take -- library_runtime.c's switch calls it for mode 1 alongside
 * func_8002BAA0 and func_8002BAAC.
 *
 * The unit implements it as a top-level asm block rather than as C, so this
 * prototype is what the compiler sees on both sides; the block's own
 * `.reloc .-4, R_MIPS_26, func_8002A788` resolves the call. */
void func_8002A788(u8 *state);

/* The card id under the library grid cursor, computed from
 * gCardGrid_bCursorRow and gCardGrid_bCursorColumn.
 *
 * func_8002A2F4.c used to declare it `s32 (u8 *)` and pass its own record.
 * The definition names no parameter, and the argument was not load bearing:
 * the record is func_8002A2F4's first parameter, so it is still in $a0 at this,
 * the function's first call, and dropping it builds byte for byte. */
s32 func_8002A6B8(void);

#endif
