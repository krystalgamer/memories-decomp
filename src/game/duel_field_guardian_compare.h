#ifndef MEMORIES_DECOMP_DUEL_FIELD_GUARDIAN_COMPARE_H
#define MEMORIES_DECOMP_DUEL_FIELD_GUARDIAN_COMPARE_H

#include "../types.h"
#include "duel_grid.h"

/* Compares the guardian-star matchup between the two field slots the given
 * cursors point at, and reports it as the display code's three-way order:
 * 4 when neither side has the advantage, 1 when the first slot loses and 6
 * when it wins.
 *
 * Both parameters are read the same way -- row and col are turned into a grid
 * index, the index selects a D_801A7AD8 record, and the pair goes to
 * Duel_CalcGuardianStarBonus -- so both are DuelFieldCursor, which is what
 * the definition says.
 *
 * Its one caller holds neither argument as that type. duel_field_display_
 * objects.c passes a DuelFieldDisplaySource *, whose x and y are the same
 * bytes at +0x0F and +0x10 that this view calls col and row, and an element
 * of the u8 array D_800E9F48. Both are stated as casts at the call rather
 * than widened here, because the fields this function reads are the cursor's,
 * not that record's. */
s32 func_80023090(DuelFieldCursor *cursor_a, DuelFieldCursor *cursor_b);

#endif
