#include "../../types.h"

/* SLPM-86398 build of src/game/main_reset_frontend_runtime.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0C0 gJapanese_D_8009B0C0
#define func_80035A64 Japanese_func_80035A64

#include "../main_reset_frontend_runtime.c"
