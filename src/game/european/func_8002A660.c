#include "../../types.h"

/* SLES-03947 build of src/game/library_update_grid_scroll.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define LIBRARY_VIEWPORT_FOLLOW_BOTTOM 0xC0

#include "../library_update_grid_scroll.c"
