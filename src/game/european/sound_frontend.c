#include "../../types.h"

/* SLES-03947 build of src/game/sound_frontend.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_800473CC func_80047658

#include "../sound_frontend.c"
