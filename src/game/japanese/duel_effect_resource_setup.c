#include "../../types.h"

/* SLPM-86398 build of src/game/duel_effect_resource_setup.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0F4_abs gJapanese_FileTransferFlags

#include "../duel_effect_resource_setup.c"
