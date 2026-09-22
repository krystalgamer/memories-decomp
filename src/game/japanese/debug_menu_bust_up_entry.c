#include "../../types.h"

/* SLPM-86398 build of src/game/debug_menu_bust_up_entry.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B2DE gJapanese_D_8009B2DE
#define D_8009B2E4 gJapanese_D_8009B2E4

#include "../debug_menu_bust_up_entry.c"
