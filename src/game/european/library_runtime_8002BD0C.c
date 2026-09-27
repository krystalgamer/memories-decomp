#include "../../types.h"

/* SLES-03947 build of src/game/library_runtime.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_8002BD0C

/* The US build reaches D_8009B0F4 through a second .data view named
 * D_8009B0F4_abs; this build names the object only. */
#define D_8009B0F4_abs D_8009B0F4

/* The splat name of its European address, which library_runtime_8002BFCC's
 * wrapper already calls it by. */
#define func_8002BD0C func_8002BD4C

/* The language index is read through %hi/%lo, as in
 * duel_effect_resource_setup's wrapper. */
#define D_8009C02B_IN_DATA

#include "../library_runtime.c"
