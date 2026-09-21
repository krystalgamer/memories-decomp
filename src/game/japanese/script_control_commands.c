#include "../../types.h"

/* SLPM-86398 build of src/game/script_control_commands.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B278 gJapanese_D_8009B278
#define D_8009B290 gJapanese_D_8009B290

#include "../script_control_commands.c"
