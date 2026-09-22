#include "../../types.h"

/* SLPM-86398 build of src/game/main_run_credits.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0C0 gJapanese_D_8009B0C0
#define D_8009B26C gJapanese_bActiveMainMode
#define D_800EB0F8 gJapanese_D_800EB0F8

#include "../main_run_credits.c"
