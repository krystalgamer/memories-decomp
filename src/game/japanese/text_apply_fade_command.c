#include "../../types.h"

/* SLPM-86398 build of src/game/text_apply_fade_command.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF74 gJapanese_D_8009AF74
#define D_8009B140 gJapanese_D_8009B140

#include "../text_apply_fade_command.c"
