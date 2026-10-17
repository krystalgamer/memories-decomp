#ifndef MEMORIES_DECOMP_FUNC_8001D5B4_H
#define MEMORIES_DECOMP_FUNC_8001D5B4_H

#include "../types.h"
#include "duel_grid.h"

/* Advances toward the signed target row/column globals, returning 1 while
 * blocked, moving or off target, and 0 when idle at the target. It schedules
 * row movement before column movement. The checked direction-forwarding
 * call and signed-byte views remain part of the matching contract. */
s32 DuelCursor_StepTowardTarget(DuelFieldCursor *cursor);

#endif
