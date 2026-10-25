#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_init_ground_projection_reference.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */

#include "../duel_init_ground_projection_reference.c"
