#include "../../types.h"

/* SLPM-86398 build of src/game/model_state_getters.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AFA0 gJapanese_D_8009AFA0

#include "../model_state_getters.c"
