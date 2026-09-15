#ifndef MEMORIES_DECOMP_FUNC_8002A788_H
#define MEMORIES_DECOMP_FUNC_8002A788_H

#include "../types.h"

/* State 1 of the library screen dispatcher, handed the same record the other
 * states take -- func_8002BAB4 in library_runtime.c calls it for mode 1
 * alongside func_8002BAA0 and func_8002BAAC.
 *
 * The handler waits for cursor motion, handles selection and cancellation,
 * then starts the next cursor move before updating scroll. */
void func_8002A788(u8 *state);

/* The card id under the library grid cursor, computed from
 * gCardGrid_bCursorRow and gCardGrid_bCursorColumn.
 *
 * The helper ignores its incoming argument. func_8002A2F4 calls it first and
 * needs no explicit argument, while func_8002A788 has already called the
 * motion stepper and passes state to restore $a0 as retail does. */
#ifdef FUNC_8002A6B8_STATE_ARGUMENT
s32 func_8002A6B8(u8 *state);
#else
s32 func_8002A6B8(void);
#endif

#endif
