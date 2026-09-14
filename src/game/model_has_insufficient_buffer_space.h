#ifndef MEMORIES_DECOMP_MODEL_HAS_INSUFFICIENT_BUFFER_SPACE_H
#define MEMORIES_DECOMP_MODEL_HAS_INSUFFICIENT_BUFFER_SPACE_H

#include "../types.h"

/* Reports whether the model buffer lacks room for the record `index` would
 * load, comparing what D_800FE240 has handed out against the shared graphics
 * packet-buffer capacity and the size the slot's own entry asks for. Non-zero
 * means there is not enough. */
s32 Model_HasInsufficientBufferSpace(s32 index, s32 type);

#endif
