#include "../../types.h"

/* SLPM-86398 build of src/game/model_scene_states.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF94 gJapanese_D_8009AF94

#include "../model_scene_states.c"
