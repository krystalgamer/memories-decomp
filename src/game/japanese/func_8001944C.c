#include "../../types.h"

/* SLPM-86398 build of src/game/func_8001944C.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800E9D70 gJapanese_D_800E9D70

#include "../func_8001944C.c"
