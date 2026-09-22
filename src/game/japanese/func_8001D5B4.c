#include "../../types.h"

/* SLPM-86398 build of src/game/func_8001D5B4.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B160 gJapanese_D_8009B160

#include "../func_8001D5B4.c"
