#ifndef MEMORIES_DECOMP_FUNC_8001B938_H
#define MEMORIES_DECOMP_FUNC_8001B938_H

#include "../types.h"
#include "duel_selection_layout.h"

/* Initializes the active side's placement cursor, restricts its row, chooses
 * a free column when available and requests the selection display. */
void Duel_BeginFieldPlacementSelection(DuelSelectionRecord *selection);

/* Restores the hand display after a scripted pick: the slot search runs inside
 * a one-pass do/while, which is the shape the inventory records for this
 * function (func_8001B938.c:92). */
void func_8001BAF0(void);

#endif
