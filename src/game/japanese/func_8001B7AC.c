#include "../../types.h"

/* SLPM-86398 build of src/game/func_8001B7AC.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800EA030 gJapanese_DuelHandDisplayRecords

#include "../func_8001B7AC.c"
