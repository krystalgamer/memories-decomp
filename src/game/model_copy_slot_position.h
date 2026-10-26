#ifndef MEMORIES_DECOMP_MODEL_COPY_SLOT_POSITION_H
#define MEMORIES_DECOMP_MODEL_COPY_SLOT_POSITION_H

#include "../types.h"

/* Retain the separate fourth halfword; it is not vector padding. */
void Model_CopySlotPosition(s32 idx, u16 *out);

/* Preserve fingerprinted overlay bodies while emitting the canonical import. */
#define Model_CopySlotU16Values Model_CopySlotPosition

#endif
