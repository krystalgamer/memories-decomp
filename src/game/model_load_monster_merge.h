#ifndef MEMORIES_DECOMP_MODEL_LOAD_MONSTER_MERGE_H
#define MEMORIES_DECOMP_MODEL_LOAD_MONSTER_MERGE_H

#include "../types.h"

/* Starts the asynchronous transfer that fills one model slot with a monster
 * merge record, and records the slot's display properties while the request
 * is in flight.
 *
 * Bit 0x80 of slot is a flag rather than part of the index. A negative model
 * id reuses the id already in the slot, and each of p2 to p5 follows the same
 * "negative means leave alone" convention.
 *
 * Returns zero once a transfer has been requested and one when the model id
 * has no record to request. Both callers discard it.
 *
 * The definition in model_load_monster_merge.c carries the full account of
 * which slots load from which file, and of the three model-id ranges that
 * have no record. */
s32 Model_LoadMonsterMerge(
    s32 slot, s32 model, s32 p2, s32 p3, s32 p4, s32 p5, s32 arg6
);

#endif
