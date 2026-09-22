#include "../../types.h"

/* SLPM-86398 build of src/game/sd_init_state.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F0 gJapanese_D_8009B0F0
#define D_8009B120 gJapanese_D_8009B120
#define g_SDValue gJapanese_SDValue

#include "../sd_init_state.c"
