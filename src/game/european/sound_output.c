#include "../../types.h"

/* SLES-03947 build of src/game/sound_output.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_800472A8 func_80047534
#define func_8004733C func_800475C8
#define func_800473CC func_80047658

#include "../sound_output.c"
