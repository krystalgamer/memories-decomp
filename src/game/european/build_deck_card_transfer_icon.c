#include "../../types.h"

/* SLES-03947 build of src/game/build_deck_card_transfer_icon.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_TYPE_ICON_FIELD_5D 0xC0
#define DUEL_CARD_TYPE_ICON_CX 0x2E0

#include "../build_deck_card_transfer_icon.c"
