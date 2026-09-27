#include "../../types.h"

/* SLES-03947 build of src/game/duel_update_card_pick_cursor.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PICK_CURSOR_FIELD_0C 0x84
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009C1B8
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009C1BD

#include "../duel_update_card_pick_cursor.c"
