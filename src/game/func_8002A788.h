#ifndef MEMORIES_DECOMP_FUNC_8002A788_H
#define MEMORIES_DECOMP_FUNC_8002A788_H

#include "../types.h"

/* State 1 of the library screen dispatcher, handed the same record the other
 * states take -- the dispatcher func_8002BAB4 (src/candidates/func_8002BAB4.c)
 * calls it for mode 1 alongside func_8002BAA0 and func_8002BAAC.
 *
 * func_8002A788 itself is generated assembly again (its old C file was a
 * `.word` transcription), so this prototype is only what its C callers see. */
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
