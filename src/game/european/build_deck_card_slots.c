#include "../../types.h"

/* SLES-03947 build of src/game/build_deck_card_slots.c. */

#define VERSION_EUROPE
#define CARD_LIST_SLOT_BOX_Y 0x2C
#define CARD_LIST_SLOT_BOX_HEIGHT 0xE0
#define CARD_LIST_SLOT_FOLLOWING_FLAG 0x80
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define BUILD_DECK_COUNT_BOX_Y 0x19
#define BUILD_DECK_COUNT_BOX_FLAGS 0x1

#include "../build_deck_card_slots.c"
