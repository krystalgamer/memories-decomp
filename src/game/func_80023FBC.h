#ifndef MEMORIES_DECOMP_FUNC_80023FBC_H
#define MEMORIES_DECOMP_FUNC_80023FBC_H

#include "../types.h"
#include "func_80023D08.h"

/* Reads the held pad bits, priority-encodes them into a direction index and
 * hands the cursor and that index to func_80023D08. It computes the second
 * argument the cursor step wants, which is what separates it from
 * func_80024088's one-argument call.
 *
 * It touches no field of the record, only forwards the pointer, so GridCursor
 * here is func_80023D08's view rather than a view of its own -- the unit had a
 * private two-field struct Obj for the same reason and nothing read either
 * field. */
void func_80023FBC(GridCursor *cursor);

#endif
