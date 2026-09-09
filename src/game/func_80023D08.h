#ifndef MEMORIES_DECOMP_FUNC_80023D08_H
#define MEMORIES_DECOMP_FUNC_80023D08_H

#include "../types.h"

/* The cursor record as this unit reads it: the grid position it keeps, the
 * row window it may move inside, and the display object it steers. The
 * callers hold their own views of the same memory -- DuelCursorStatus names
 * only the status byte at 0x19, and func_80023FBC needs nothing but the
 * pointer -- so this is the unit's view, not the whole record. */
typedef struct {
    u8 pad00[4];
    u8 *object;
    u8 pad08[7];
    s8 col;
    s8 row;
    s8 min_row;
    s8 max_row;
    u8 pad13[5];
    u8 page;
    u8 flags;
} GridCursor;

/* One step of the duel field cursor. `dir` is the direction index
 * func_80023FBC priority-encodes out of the held pad bits: negative for no
 * direction, bit 0 picking the axis and bit 1 the sign. A set 0x80 flag means
 * a move is still settling, and that arm ignores `dir` entirely.
 *
 * func_80024088 reaches it with the object alone. Its prologue assigns only
 * $a0, so `dir` arrives as whatever that function's own caller left in $a1,
 * and there is no expression duel_cursor_status.c could write for it -- the
 * call site does not compute one. The arm below preserves that old-style call
 * while func_80023FBC, which does compute the argument, gets the real
 * prototype. */
#ifdef FUNC_80023D08_AMBIENT_DIRECTION_ARG
void func_80023D08();
#else
void func_80023D08(GridCursor *o, s32 dir);
#endif

#endif
