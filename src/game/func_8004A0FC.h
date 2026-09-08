#ifndef MEMORIES_DECOMP_FUNC_8004A0FC_H
#define MEMORIES_DECOMP_FUNC_8004A0FC_H

#include "../types.h"

/* Applies a sequence voice's spatialisation to its two channel states.
 *
 * Both callers pass two views into the same record, computed as base plus a
 * per-channel offset, so the two arguments are siblings rather than unrelated
 * objects. The function returns nothing; the two declarations that gave it an
 * int return were guesses that never had a value to use. */
void func_8004A0FC(u8 *arg0, u8 *arg1);

#endif
