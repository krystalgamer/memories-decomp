#include "../../types.h"

/* SLES-03947 build of src/game/duel_card_frame_draw.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_FRAME_SCREEN_HEIGHT 0x100
#define DUEL_CARD_FRAME_CXCY 0xE10280

#include "../duel_card_frame_draw.c"
