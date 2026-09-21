#include "../../types.h"

/* SLPM-86398 build of src/game/script_flag_commands.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B284 gJapanese_D_8009B284
#define D_8009B290 gJapanese_D_8009B290
#define D_8009B294 gJapanese_D_8009B294
#define D_8009B298 gJapanese_D_8009B298
#define D_8009B29C gJapanese_D_8009B29C
#define D_8009B2A8 gJapanese_D_8009B2A8

#include "../script_flag_commands.c"
