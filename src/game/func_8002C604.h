#ifndef MEMORIES_DECOMP_FUNC_8002C604_H
#define MEMORIES_DECOMP_FUNC_8002C604_H

#include "../types.h"

/* Allocates and fills one effect request entry, storing arg0 as the id at
 * +0x18, and returns the entry or 0 when none was free. */
u8 *func_8002C604(s32 arg0);

#endif
