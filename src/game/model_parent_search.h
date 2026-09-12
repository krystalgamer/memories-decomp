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
 * func_8004CB0C walks the same storage as bytes and supplies an SDK coordinate.
 * Keep these measured pointer views explicit without changing the word ABI. */
#ifdef MODEL_PARENT_SEARCH_COORD_VIEW
s32 func_8005A3D0(u8 *model, GsCOORDUNIT *parent);
#else
s32 func_8005A3D0(ModelSlot *model, void *parent);
#endif

#endif
