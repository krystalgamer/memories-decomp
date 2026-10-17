#include "../../types.h"

/* SLES-03947 build of src/game/sound_output.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define SD_BGMPreload func_80047534

#include "../sound_output.c"
