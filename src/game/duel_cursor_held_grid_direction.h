#ifndef MEMORIES_DECOMP_DUEL_CURSOR_HELD_GRID_DIRECTION_H
#define MEMORIES_DECOMP_DUEL_CURSOR_HELD_GRID_DIRECTION_H

#include "../types.h"
#include "duel_cursor_grid_direction.h"

/* Reads the held pad bits, priority-encodes them into a direction index and
 * hands the cursor and that index to DuelCursor_ProcessGridDirection. It computes the second
 * argument the cursor step wants, which is what separates it from
 * DuelCursor_UpdateDirection's one-argument call.
 *
 * It touches no field of the selection record, only forwards the pointer. */
void DuelCursor_ProcessHeldGridDirection(DuelSelectionRecord *cursor);

#endif
