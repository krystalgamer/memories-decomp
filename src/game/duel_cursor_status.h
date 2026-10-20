#ifndef MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H
#define MEMORIES_DECOMP_DUEL_CURSOR_STATUS_H

#include "../types.h"
#include "duel_selection_layout.h"

/* The byte these two report on. They run a cursor update and then hand back
 * the status byte at 0x19, so the return is that byte widened rather than a
 * result of their own.
 *
 * The callers use different checked views of DuelCursor_UpdateDirection --
 * see the note in duel_cursor_status.c. */

/* Direction selected by the cursor path plus the target column and row used
 * by DuelCursor_StepTowardTarget. The field-action controller keeps the
 * coordinates unsigned; the comparison helper requires signed loads. */
extern s8 gDuelCursor_Direction;
#ifdef DUEL_CURSOR_COORDS_SIGNED
extern s8 gDuelCursor_TargetColumn;
extern s8 gDuelCursor_TargetRow;
#else
extern u8 gDuelCursor_TargetColumn;
extern u8 gDuelCursor_TargetRow;
#endif

s32 DuelCursor_UpdateFromInput(DuelSelectionRecord *object);

#ifdef FUNC_80024088_WIDE_DIRECTION
s32 DuelCursor_UpdateDirection(DuelSelectionRecord *cursor, s8 dir);
#else
s32 DuelCursor_UpdateDirection(DuelSelectionRecord *object);
#endif

#endif
