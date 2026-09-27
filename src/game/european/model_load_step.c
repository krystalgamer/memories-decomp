#include "../../types.h"

/* SLES-03947 build of src/game/model_load_step.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_8004D75C func_80051C6C
#define func_8004D914 func_80051E24

#include "../model_load_step.c"
