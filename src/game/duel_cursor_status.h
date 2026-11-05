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
 * by DuelCursor_StepTowardTarget. The field-action controller treats the
 * coordinates as unsigned; the step helper compares them as signed bytes. */
extern s8 gDuelCursor_Direction;
extern u8 gDuelCursor_TargetColumn;
extern u8 gDuelCursor_TargetRow;

s32 DuelCursor_UpdateFromInput(DuelSelectionRecord *object);

s32 DuelCursor_UpdateDirection(DuelSelectionRecord *object);
/* Callers that also load the direction into $a1 for the ambient
 * DuelCursor_ProcessGridDirection argument call through this type. */
typedef s32 (*DuelCursorUpdateDirectionWithDir)(DuelSelectionRecord *cursor, s8 dir);

#endif
