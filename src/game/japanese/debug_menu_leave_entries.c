#include "../../types.h"

/* SLPM-86398 build of src/game/debug_menu_leave_entries.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B26C gJapanese_bActiveMainMode
#define D_8009B2B2 gJapanese_D_8009B2B2
#define func_80035A64 Japanese_func_80035A64
#define gDebugMenu_bCursor gDebugMenu_bCursor

#include "../debug_menu_leave_entries.c"
