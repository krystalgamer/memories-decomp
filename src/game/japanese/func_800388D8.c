#include "../../types.h"

/* SLPM-86398 build of src/game/func_800388D8.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF74 gJapanese_D_8009AF74
#define D_8009B140 gJapanese_D_8009B140
#define gFade_State gJapanese_FadeState

#include "../func_800388D8.c"
