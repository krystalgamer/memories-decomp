#ifndef MEMORIES_DECOMP_FUNC_8002A788_H
#define MEMORIES_DECOMP_FUNC_8002A788_H

#include "../types.h"

/* State 1 of the library screen dispatcher, handed the same record the other
 * states take -- the dispatcher func_8002BAB4 (src/candidates/func_8002BAB4.c)
 * calls it for mode 1 alongside func_8002BAA0 and func_8002BAAC.
 *
 * The handler waits for cursor motion, handles selection and cancellation,
 * then starts the next cursor move before updating scroll. */
void func_8002A788(u8 *state);

/* The card id under the library grid cursor, computed from
 * gCardGrid_bCursorRow and gCardGrid_bCursorColumn.
 *
 * func_8002A2F4.c used to declare it `s32 (u8 *)` and pass its own record.
 * The definition names no parameter, and the argument was not load bearing:
 * the record is func_8002A2F4's first parameter, so it is still in $a0 at this,
 * the function's first call, and dropping it builds byte for byte. */
/* Unlike the first-call case above, func_8002A788 has already called the
 * motion stepper. Its explicit state argument restores $a0 as retail does;
 * the card-id definition still ignores that incoming value. */
#ifdef FUNC_8002A6B8_STATE_ARGUMENT
s32 func_8002A6B8(u8 *state);
#else
s32 func_8002A6B8(void);
#endif

#endif
