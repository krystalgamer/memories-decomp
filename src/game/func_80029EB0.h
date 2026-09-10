#ifndef MEMORIES_DECOMP_FUNC_80029EB0_H
#define MEMORIES_DECOMP_FUNC_80029EB0_H

#include "../types.h"

/* The flags byte at +0x56 of entry `index` in a four-byte-stride table based
 * at `base`. func_8002A2F4 tests its 0x80 bit for the selected card, and reads
 * bit 0 of the same byte inline. */
unsigned int func_80029EB0(unsigned char *base, int index);

#endif
