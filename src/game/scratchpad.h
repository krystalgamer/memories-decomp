#ifndef MEMORIES_DECOMP_SCRATCHPAD_H
#define MEMORIES_DECOMP_SCRATCHPAD_H

#include "../types.h"

/* Address of a byte offset inside the 1 KB PSX data-cache scratchpad. */
#define SCRATCH_ADDR(off) ((void *)((u8 *)0x1F800000 + (off)))

#endif
