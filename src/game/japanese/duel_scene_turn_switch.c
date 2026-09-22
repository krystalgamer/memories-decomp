#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene_turn_switch.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_800907D8 gJapanese_D_800907D8

#include "../duel_scene_turn_switch.c"
