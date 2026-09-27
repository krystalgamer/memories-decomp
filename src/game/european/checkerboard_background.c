#include "../../types.h"

/* SLES-03947 build of src/game/checkerboard_background.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define CHECKERBOARD_CLUT_X 0x340
#define CHECKERBOARD_SCREEN_HEIGHT 0x100

#include "../checkerboard_background.c"
