#include "../../types.h"

/* SLES-03947 build of src/game/library_card_grid.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define LIBRARY_GRID_TEXTURE_WORD 0xE702B0
#define LIBRARY_GRID_SCREEN_HEIGHT 0x100
#define LIBRARY_GRID_SKIP_CY_STORE
#define LIBRARY_CARD_TEXT_BOX_Y 0xD5

#include "../library_card_grid.c"
