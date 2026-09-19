#ifndef MEMORIES_DECOMP_FUNC_80029EB0_H
#define MEMORIES_DECOMP_FUNC_80029EB0_H

#include "../types.h"

/* The flags byte at +0x56 of card `index` in a four-byte-stride Library state
 * table based at `base`. */
unsigned int Library_GetCardFlags(unsigned char *base, int index);

#endif
