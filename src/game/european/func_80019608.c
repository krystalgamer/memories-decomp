#include "../../types.h"

/* SLES-03947 build of src/game/func_80019608.c. The US build reaches these objects
 * through second .data views named *_abs; the European build names the
 * objects only. The US source is included as is. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_USE_OBJECT_Y 0xE

#include "../func_80019608.c"
