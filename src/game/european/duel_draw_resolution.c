#include "../../types.h"

/* SLES-03947 build of src/game/duel_draw_resolution.c. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_DRAW_RESOLUTION_CARD_Y 0xA2
#define DUEL_CARD_READBACK_HEIGHT 0xD4

#include "../duel_draw_resolution.c"
#include "../duel_field_card_render_data.h"
