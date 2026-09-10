#ifndef MEMORIES_DECOMP_FUNC_80023D08_H
#define MEMORIES_DECOMP_FUNC_80023D08_H

#include "../types.h"

/* Motion state of the display object steered by GridCursor: current and
 * target coordinates, two per-frame steps, the remaining frame count, and
 * the moving latch. */
typedef struct {
    u8 pad00[0x28];
    s16 x;
    s16 y;
    s16 target_x;
    s16 target_y;
    u8 pad30[6];
    u16 step_x;
    u8 pad38[2];
    u16 step_y;
    u8 pad3C[0x24];
    u16 steps;
    u8 pad62[0xA];
    u8 moving;
} DuelFieldCursorObject;

#define DUEL_FIELD_CURSOR_OBJECT_OFFSET(member) \
    ((u32)&(((DuelFieldCursorObject *)0)->member))

typedef char DuelFieldCursorObject_x_offset_must_be_0x28[
    DUEL_FIELD_CURSOR_OBJECT_OFFSET(x) == 0x28 ? 1 : -1
];
typedef char DuelFieldCursorObject_step_x_offset_must_be_0x36[
    DUEL_FIELD_CURSOR_OBJECT_OFFSET(step_x) == 0x36 ? 1 : -1
];
typedef char DuelFieldCursorObject_steps_offset_must_be_0x60[
    DUEL_FIELD_CURSOR_OBJECT_OFFSET(steps) == 0x60 ? 1 : -1
];
typedef char DuelFieldCursorObject_moving_offset_must_be_0x6C[
    DUEL_FIELD_CURSOR_OBJECT_OFFSET(moving) == 0x6C ? 1 : -1
];

#undef DUEL_FIELD_CURSOR_OBJECT_OFFSET

/* The cursor record as this unit reads it: the grid position it keeps, the
 * row window it may move inside, and the display object it steers. The
 * callers hold their own views of the same memory -- DuelCursorStatus names
 * only the status byte at 0x19, and func_80023FBC needs nothing but the
 * pointer -- so this is the unit's view, not the whole record. */
typedef struct {
    u8 pad00[4];
    DuelFieldCursorObject *object;
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
 * call site does not compute one. The arm below preserves that one-argument
 * call while func_80023FBC, which does compute the argument, gets the real
 * prototype. */
#ifdef FUNC_80023D08_AMBIENT_DIRECTION_ARG
void func_80023D08(GridCursor *o);
#else
void func_80023D08(GridCursor *o, s32 dir);
#endif

#endif
