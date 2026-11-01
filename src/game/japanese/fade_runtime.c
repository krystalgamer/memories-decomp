#include "../../types.h"

/* SLPM-86398 build of src/game/fade_runtime.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B0D8 gJapanese_FrameStep
#define D_8009B140 gJapanese_D_8009B140
#define D_800E9D94 gJapanese_D_800E9D94

#include "../fade_runtime.c"
