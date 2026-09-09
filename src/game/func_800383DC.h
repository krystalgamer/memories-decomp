#ifndef MEMORIES_DECOMP_FUNC_800383DC_H
#define MEMORIES_DECOMP_FUNC_800383DC_H

#include "../types.h"

/* D_80090EAC entry: resolves the text bank pointer for the string id in
 * D_8009B32E -- above 0xCFFF from D_801C0000, above the global-string base from
 * D_801D5800, otherwise the campaign bank -- and hands back the slot it found.
 *
 * Two things diverge from the table's entry type and both are the definition's:
 * it RETURNS `u32 *`, which no caller reads, and its parameter is this unit's
 * own private `struct Obj`. The tag stays incomplete here, so only the table
 * consumes this header -- a file with its own `struct Obj` would silently read
 * the prototype against that -- and the table entry casts. */
struct Obj;
u32 *func_800383DC(struct Obj *object);

#endif
