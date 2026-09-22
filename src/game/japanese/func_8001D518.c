#include "../../types.h"

/* SLPM-86398 build of src/game/func_8001D518.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B1F8 gJapanese_D_8009B1F8

#include "../func_8001D518.c"
