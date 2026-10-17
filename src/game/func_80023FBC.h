#ifndef MEMORIES_DECOMP_FUNC_80023FBC_H
#define MEMORIES_DECOMP_FUNC_80023FBC_H

#include "../types.h"
#include "func_80023D08.h"

/* Reads the held pad bits, priority-encodes them into a direction index and
 * hands the cursor and that index to DuelCursor_ProcessGridDirection. It computes the second
 * argument the cursor step wants, which is what separates it from
 * DuelCursor_UpdateDirection's one-argument call.
 *
 * It touches no field of the selection record, only forwards the pointer. */
void DuelCursor_ProcessHeldGridDirection(DuelSelectionRecord *cursor);

#endif
