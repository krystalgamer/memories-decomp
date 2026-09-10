#ifndef MEMORIES_DECOMP_FUNC_8003A198_H
#define MEMORIES_DECOMP_FUNC_8003A198_H

#include "../types.h"

/* Walks three levels of halfword offset tables rooted at `b`: entry `x` of the
 * root gives the offset of a second table, entry `y` of that gives a third,
 * and entry `z` of the third must be non-zero. Returns 1 when all three are
 * non-zero, 0 at the first zero. func_8003A1EC asks it about the table at
 * D_801AF000 or D_801AF800. */
int func_8003A198(unsigned char *b, int x, int y, int z);

#endif
