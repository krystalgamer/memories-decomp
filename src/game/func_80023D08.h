#ifndef MEMORIES_DECOMP_FUNC_80023D08_H
#define MEMORIES_DECOMP_FUNC_80023D08_H

#include "../types.h"
#include "display_object.h"

#define DUEL_CURSOR_MOTION_OFFSET(member) \
    ((u32)&(((DisplayObject *)0)->member))

typedef char DuelCursorMotion_position_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(position.h.field_28) == 0x28 &&
    DUEL_CURSOR_MOTION_OFFSET(position.h.field_2A) == 0x2A ? 1 : -1
];
typedef char DuelCursorMotion_target_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_2C.h.field_2C) == 0x2C &&
    DUEL_CURSOR_MOTION_OFFSET(field_2C.h.field_2E) == 0x2E ? 1 : -1
];
typedef char DuelCursorMotion_step_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_34.h.field_36) == 0x36 &&
    DUEL_CURSOR_MOTION_OFFSET(field_38.h.field_3A) == 0x3A ? 1 : -1
];
typedef char DuelCursorMotion_control_offsets_must_match[
    DUEL_CURSOR_MOTION_OFFSET(field_60) == 0x60 &&
    DUEL_CURSOR_MOTION_OFFSET(field_6C) == 0x6C ? 1 : -1
];

#undef DUEL_CURSOR_MOTION_OFFSET

/* The cursor record as this unit reads it: the grid position it keeps, the
 * row window it may move inside, and the display object it steers. The
 * callers hold their own views of the same memory -- DuelCursorStatus names
 * only the status byte at 0x19, and func_80023FBC needs nothing but the
 * pointer -- so this is the unit's view, not the whole record. */
typedef struct {
    u8 pad00[4];
    DisplayObject *G32 object;
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
 * DuelCursor_UpdateDirection reaches it with the object alone. Its prologue
 * assigns only $a0, so `dir` arrives as whatever that function's own caller
 * left in $a1, and there is no expression duel_cursor_status.c could write for
 * it -- the call site does not compute one. The arm below preserves that
 * one-argument call while func_80023FBC, which does compute the argument, gets
 * the real prototype. */
#ifdef FUNC_80023D08_AMBIENT_DIRECTION_ARG
void func_80023D08(GridCursor *o);
#else
void func_80023D08(GridCursor *o, s32 dir);
#endif

#endif
