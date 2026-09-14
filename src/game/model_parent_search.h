#ifndef MEMORIES_DECOMP_MODEL_PARENT_SEARCH_H
#define MEMORIES_DECOMP_MODEL_PARENT_SEARCH_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

/* Searches the coordinate parent/backlink relationships and returns an entry
 * index. The implementation uses the model layout and an opaque parent;
 * func_8004CB0C passes its slot and an SDK coordinate. */
s32 func_8005A3D0(ModelSlot *model, void *parent);

#endif
