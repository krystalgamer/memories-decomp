#include "../../types.h"

/* SLES-03947 build of src/game/library_draw_wireframe_globe.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define GLOBE_GEOM_OFFSET_Y 0x68

#include "../library_draw_wireframe_globe.c"
