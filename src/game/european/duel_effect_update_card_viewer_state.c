#include "../../types.h"

/* SLES-03947 build of src/game/func_800283F4.c, with its European body. */

#define VERSION_EUROPE
#define D_8009B248 gDuel_bEffectHandlerFlags
/* The US build reaches these objects through second .data views named
 * *_abs; this build names the objects only. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

#include "../func_800283F4.c"
