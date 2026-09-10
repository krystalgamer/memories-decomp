#ifndef MEMORIES_DECOMP_FUNC_8004DC38_H
#define MEMORIES_DECOMP_FUNC_8004DC38_H

#include "../types.h"
#include "model.h"

/* Returns the sequence row and accumulated position for one model channel,
   packed into the high byte and low 24 bits respectively. */
s32 func_8004DB14(ModelSlot *slot, s32 channel);

/* Seeks channel i of a model slot to position pos within sequence n.
 *
 * An earlier note here said the definition could not take a ModelSlot,
 * because introducing the cast changed the entry instruction. That was the
 * member spelling, not the type. `(u8 *)p->field_2C8 + offset` reassociates
 * so that the slot pointer becomes the second operand of the addu, and
 * retail has it first. A plain `p->field_2C8[n][i]` index keeps it first, as
 * long as the two channel offsets are still computed up front for cse to
 * reuse. The definition takes the typed slot and all three callers pass it
 * directly. */
void func_8004DC38(ModelSlot *slot, s32 i, s32 n, u32 pos);

#endif
