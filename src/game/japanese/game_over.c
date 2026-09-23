#include "../../types.h"

/* SLPM-86398 build of src/game/game_over.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define GameOver_Init func_8003BFA4
#define func_8003CA5C func_8003C0B0
#define gFade_State gJapanese_FadeState

#include "../game_over.c"
