#ifndef MEMORIES_DECOMP_FUNC_80023D08_H
#define MEMORIES_DECOMP_FUNC_80023D08_H

#include "../types.h"
#include "display_object.h"
#include "duel_selection_layout.h"

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

#define DUEL_CURSOR_RECORD_OFFSET(member) \
    ((u32)&(((DuelSelectionRecord *)0)->member))

typedef char DuelCursor_record_offsets_must_match[
    DUEL_CURSOR_RECORD_OFFSET(cursor_object) == 0x04 &&
    DUEL_CURSOR_RECORD_OFFSET(col) == 0x0F &&
    DUEL_CURSOR_RECORD_OFFSET(row) == 0x10 &&
    DUEL_CURSOR_RECORD_OFFSET(row_min) == 0x11 &&
    DUEL_CURSOR_RECORD_OFFSET(row_end) == 0x12 &&
    DUEL_CURSOR_RECORD_OFFSET(field_18) == 0x18 &&
    DUEL_CURSOR_RECORD_OFFSET(status) == 0x19 ? 1 : -1
];

#undef DUEL_CURSOR_RECORD_OFFSET

/* One step of the duel field cursor. `dir` is the direction index
 * DuelCursor_ProcessHeldGridDirection priority-encodes out of the held pad bits: negative for no
 * direction, bit 0 picking the axis and bit 1 the sign. A set 0x80 flag means
 * a move is still settling, and that arm ignores `dir` entirely.
 *
 * DuelCursor_UpdateDirection reaches it with the object alone. Its prologue
 * assigns only $a0, so `dir` arrives as whatever that function's own caller
 * left in $a1, and there is no expression duel_cursor_status.c could write for
 * it -- the call site does not compute one. The arm below preserves that
 * one-argument call while DuelCursor_ProcessHeldGridDirection, which does compute the argument, gets
 * the real prototype. */
#ifdef FUNC_80023D08_AMBIENT_DIRECTION_ARG
void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o);
#else
void DuelCursor_ProcessGridDirection(DuelSelectionRecord *o, s32 dir);
#endif

#endif
