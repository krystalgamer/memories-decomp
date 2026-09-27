#include "../../types.h"

/* SLES-03947 build of src/game/mem_card_driver.c: the symbols below sit at other addresses in the
 * European executable and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AF7C gEuropean_D_8009AF7C

#include "../mem_card_driver.c"
