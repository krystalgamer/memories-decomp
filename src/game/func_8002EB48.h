#ifndef MEMORIES_DECOMP_FUNC_8002EB48_H
#define MEMORIES_DECOMP_FUNC_8002EB48_H

#include "../types.h"

/* D_80090C50 handler: takes one byte from the stream into D_8009B363 and puts
 * the scene mode at 5. It does not consult the busy latch, so it runs once per
 * dispatch and consumes a byte every time. */
void func_8002EB48(void);

#endif
