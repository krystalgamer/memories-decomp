#ifndef MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H
#define MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H

#include "../types.h"

/* The byte these two report on. They run a cursor update and then hand back
 * the status byte at 0x19, so the return is that byte widened rather than a
 * result of their own.
 *
 * This describes only what these two functions touch. It is not the cursor
 * record: duel_card_pick_cursor.h's DuelCardPickCursor and func_8001D5B4.c
 * each hold a fuller view of the same memory.
 *
 * Neither caller consumes this header, on purpose -- see the note in
 * duel_cursor_status.c. */
typedef struct {
    u8 pad_00[0x19];
    u8 status;
} DuelCursorStatus;

s32 func_80024060(DuelCursorStatus *object);
s32 func_80024088(DuelCursorStatus *object);

#endif
