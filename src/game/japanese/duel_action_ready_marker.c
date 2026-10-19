#include "../../types.h"

/* SLPM-86398 build of src/game/duel_action_ready_marker.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B1F8 gJapanese_D_8009B1F8

#include "../duel_action_ready_marker.c"
