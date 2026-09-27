#include "../../types.h"

/* SLES-03947 build of src/game/main_run_animated_battle.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define ANIMATED_BATTLE_GEOM_OFFSET_Y 0x80

#include "../main_run_animated_battle.c"
