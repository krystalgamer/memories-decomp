#include "../../types.h"

/* SLES-03947 build of src/game/display_effect_resource_setup.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DISPLAY_EFFECT_CLUT_X 640
#define DISPLAY_EFFECT_SLOT_ID(i) (gDisplayEffect_CachedResourceIds[i])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 17249

#include "../display_effect_resource_setup.c"
