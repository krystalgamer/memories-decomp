#include "../../types.h"

/* SLES-03947 build of src/game/duel_scene_update.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_QUIT_BOX_FLAGS 0x40
#define DUEL_QUIT_BOX_HEIGHT 0x30

/* The US block defines these two together with the height; they are the US
 * values. */
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50

#include "../duel_scene_update.c"
