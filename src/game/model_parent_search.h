#ifndef MEMORIES_DECOMP_MODEL_PARENT_SEARCH_H
#define MEMORIES_DECOMP_MODEL_PARENT_SEARCH_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

/* Returns the index of the first coordinate entry whose super is `parent`
 * and which is itself the super of some entry -- the first child of
 * `parent` that branches further -- or entry_count when there is none.
 * Model_InitSlot passes two entries from the model's coordinate-unit array
 * as the parent to find. */
s32 Model_FindBranchingChildCoord(ModelSlot *model, GsCOORDUNIT *parent);

#endif
