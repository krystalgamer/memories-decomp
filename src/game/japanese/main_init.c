#include "../../types.h"

/* SLPM-86398 build of src/game/main_init.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define VERSION_JAPAN
#define D_8009B0D8 gJapanese_FrameStep

#include "../main_init.c"
