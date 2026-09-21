#include "../../types.h"

/* SLPM-86398 build of src/game/model_state_setters.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF92 gJapanese_D_8009AF92
#define D_8009AFA4 gJapanese_D_8009AFA4

#include "../model_state_setters.c"
