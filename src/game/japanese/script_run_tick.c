#include "../../types.h"

/* SLPM-86398 build of src/game/script_run_tick.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B290 gJapanese_D_8009B290
#define D_8009B2A4 gJapanese_D_8009B2A4

#include "../script_run_tick.c"
