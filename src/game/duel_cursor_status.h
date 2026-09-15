#ifndef MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H
#define MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H

#include "../types.h"
#include "duel_grid.h"

/* The byte these two report on. They run a cursor update and then hand back
 * the status byte at 0x19, so the return is that byte widened rather than a
 * result of their own.
 *
 * This describes only what these two functions touch. It is not the cursor
 * record: duel_card_pick_cursor.h's DuelCardPickCursor and func_8001D5B4.c
 * each hold a fuller view of the same memory.
 *
 * The callers use different checked views of func_80024088 -- see the note in
 * duel_cursor_status.c. */
typedef struct {
    u8 pad_00[0x19];
    u8 status;
} DuelCursorStatus;

/* Direction selected by the cursor path plus the target column and row used
   by func_8001D5B4. The field-action candidate keeps the coordinates
   unsigned; the comparison helper requires signed loads. */
extern s8 D_8009B160;
#ifdef DUEL_CURSOR_COORDS_SIGNED
extern s8 D_8009B1D6;
extern s8 D_8009B1D7;
#else
extern u8 D_8009B1D6;
extern u8 D_8009B1D7;
#endif

s32 func_80024060(DuelCursorStatus *object);

#ifdef FUNC_80024088_WIDE_DIRECTION
s32 func_80024088(DuelFieldCursor *cursor, s8 dir);
#else
s32 func_80024088(DuelCursorStatus *object);
#endif

#endif
