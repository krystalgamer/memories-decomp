#include "../../types.h"

/* SLES-03947 build of src/game/text_box_layout_helpers.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define TEXT_BOX_LAYOUT_QUAD_FLAG 0x40
#define TEXT_BOX_LAYOUT_HEIGHT 0xC0

#include "../text_box_layout_helpers.c"
