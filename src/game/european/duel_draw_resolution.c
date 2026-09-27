#include "../../types.h"

/* SLES-03947 build of src/game/duel_draw_resolution.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_DRAW_RESOLUTION_CARD_Y 0xA2

#include "../duel_draw_resolution.c"
