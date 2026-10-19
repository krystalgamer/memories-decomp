#include "../../types.h"

/* SLES-03947 build of src/game/model_init_slot.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define tent_Model_RebaseTexturedPrimitives func_80051610
#define Model_ResetSlotAnimationTables func_80051A9C

#include "../model_init_slot.c"
