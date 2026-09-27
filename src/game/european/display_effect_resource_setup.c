#include "../../types.h"

/* SLES-03947 build of src/game/display_effect_resource_setup.c. The US build reaches these objects
 * through second .data views named *_abs; the European build names the
 * objects only. The US source is included as is. */
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DISPLAY_EFFECT_CLUT_X 640
#define DISPLAY_EFFECT_SLOT_ID(i) (D_8015C410[i])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 17249

#include "../display_effect_resource_setup.c"
