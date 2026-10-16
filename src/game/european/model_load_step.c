#include "../../types.h"

/* SLES-03947 build of src/game/model_load_step.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define Model_BuildSlotAnimationTables func_80051C6C
#define Model_LinkSlotAnimationRows func_80051E24

#include "../model_load_step.c"
