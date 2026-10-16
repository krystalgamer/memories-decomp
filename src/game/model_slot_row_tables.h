#ifndef MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H
#define MEMORIES_DECOMP_MODEL_SLOT_ROW_TABLES_H

#include "../types.h"

s32 func_8004D134(s32 mode, u16 *kind, u8 *ctx, s32 *best, s32 *total);

/* Clears animation keys/durations and imports the mapped owner-block payloads. */
void Model_ResetSlotAnimationTables(s32 slot, u8 *commands);

/* Indexes each part's animation rows and computes their duration maxima. */
void Model_BuildSlotAnimationTables(s32 slot);

#endif
