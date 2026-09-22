#include "../../types.h"

/* SLPM-86398 build of src/game/model_keyframe_update.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B074 gJapanese_D_8009B074
#define D_8009B078 gJapanese_D_8009B078
#define D_8009B07C gJapanese_D_8009B07C

#include "../model_keyframe_update.c"
