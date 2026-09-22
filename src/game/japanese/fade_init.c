#include "../../types.h"

/* SLPM-86398 build of src/game/fade_init.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define gFade_State gJapanese_FadeState

#include "../fade_init.c"
