#ifndef MEMORIES_DECOMP_FUNC_8003787C_H
#define MEMORIES_DECOMP_FUNC_8003787C_H

#include "../types.h"

/* One D_80090E64 entry, the sibling of func_800378D8: same field81/bit 0x80
 * gating, but it also runs func_80039FD4 on D_8009B328 before clearing field81
 * when that record's field51 is zero.
 *
 * The parameter keeps the incomplete `struct Obj *` the table already used. The
 * tag is this unit's own private record and stays private -- a file that
 * includes this header and defines its own `struct Obj` would silently read the
 * prototype against that instead, so only the table consumes it. */
struct Obj;
void func_8003787C(struct Obj *object);

#endif
