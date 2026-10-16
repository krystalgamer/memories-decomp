#ifndef MEMORIES_DECOMP_FUNC_80029EB0_H
#define MEMORIES_DECOMP_FUNC_80029EB0_H

#include "../types.h"
#include "library_state.h"

/* Returns the flags of the directly indexed card record, including ID zero. */
u32 Library_GetCardFlags(const LibraryState *base, s32 index);

#endif
