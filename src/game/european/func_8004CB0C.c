#include "../../types.h"

/* SLES-03947 build of src/game/func_8004CB0C.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define func_8004D134 func_80051610
#define func_8004D58C func_80051A9C

#include "../func_8004CB0C.c"
