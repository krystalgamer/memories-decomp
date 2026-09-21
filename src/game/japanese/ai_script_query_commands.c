#include "../../types.h"

/* SLPM-86398 build of src/game/ai_script_query_commands.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_80011994 gJapanese_D_80011994

#include "../ai_script_query_commands.c"
