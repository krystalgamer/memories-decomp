#include "../../types.h"

/* SLES-03947 build of src/game/func_80018FEC.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009B23A gDuel_wSceneStateFlags
#define func_800472A8 func_80047534

#include "../func_80018FEC.c"
