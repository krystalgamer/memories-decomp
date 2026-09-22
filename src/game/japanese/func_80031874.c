#include "../../types.h"

/* SLPM-86398 build of src/game/func_80031874.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gBuildDeck_pState gBuildDeck_pState

#include "../func_80031874.c"
