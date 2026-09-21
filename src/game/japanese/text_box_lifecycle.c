#include "../../types.h"

/* SLPM-86398 build of src/game/text_box_lifecycle.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_80035CA8 DuelEffect_ClearOccupancyValue

#include "../text_box_lifecycle.c"
