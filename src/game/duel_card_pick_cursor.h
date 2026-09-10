#ifndef MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H
#define MEMORIES_DECOMP_DUEL_CARD_PICK_CURSOR_H

#include "../types.h"

/* The complete cursor view used by Duel_UpdateCardPickCursor. The column,
 * row, and status byte agree with the narrower field-grid and status views. */
typedef struct {
    u8 pad_00[0x0C];
    s16 field_0C;
    u8 pad_0E;
    s8 col;
    s8 row;
    u8 field_11;
    u8 field_12;
    u8 field_13;
    u8 pad_14[4];
    u8 field_18;
    u8 status;
} DuelCardPickCursor;

#define DUEL_CARD_PICK_CURSOR_OFFSET(member) \
    ((u32)&(((DuelCardPickCursor *)0)->member))

typedef char DuelCardPickCursor_col_offset_must_be_0xF[
    DUEL_CARD_PICK_CURSOR_OFFSET(col) == 0xF ? 1 : -1
];
typedef char DuelCardPickCursor_row_offset_must_be_0x10[
    DUEL_CARD_PICK_CURSOR_OFFSET(row) == 0x10 ? 1 : -1
];
typedef char DuelCardPickCursor_status_offset_must_be_0x19[
    DUEL_CARD_PICK_CURSOR_OFFSET(status) == 0x19 ? 1 : -1
];
typedef char DuelCardPickCursor_size_must_be_0x1A[
    sizeof(DuelCardPickCursor) == 0x1A ? 1 : -1
];

#undef DUEL_CARD_PICK_CURSOR_OFFSET

/* The mode byte for the "pick a card off the field" cursor, at 0x2CC($gp).
 * Duel_UpdateCardPickCursor owns it, and its own comment gives the bits:
 *
 *   0x80  the cursor has been initialised for this activation
 *   0x40  a pick has just been committed; hold until D_8009B162 expires
 *   0x20  the whole cursor mode should be torn down once 0x40 clears
 *
 * Zero therefore means the cursor is not active at all, which is how
 * func_800179F4 uses it: it clears this byte in the run of assignments that
 * resets the duel, alongside D_8009B162, the countdown the 0x40 hold waits
 * on.
 *
 * func_8001BD88 is the third accessor and is still assembly; it reads and
 * writes the byte gp-relative like everything else here. */
extern u8 D_8009B1D4;

void Duel_UpdateCardPickCursor(DuelCardPickCursor *cursor);

#endif
