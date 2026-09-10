#ifndef MEMORIES_DECOMP_FUNC_80058DD8_H
#define MEMORIES_DECOMP_FUNC_80058DD8_H

#include "../types.h"

/* Reports a model slot's readiness by index into D_800F2C40. It returns 2
 * while the slot's field_E14 is not 0xFF, and otherwise 0 or 1 from
 * field_E1F, so 2 means "not settled yet" rather than a third state the
 * caller acts on: func_8002BAB4 only tests for 1.
 *
 * The address-based name is kept. The slot fields it reads are still
 * field_E14 and field_E1F in model.h, so a name here would claim more about
 * what is being reported than those two offsets currently support. */
int func_80058DD8(int index);

#endif
