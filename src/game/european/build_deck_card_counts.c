#include "../../types.h"

/* SLES-03947 build of src/game/build_deck_card_counts.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define BUILD_DECK_COUNT_BOX_Y 0x19
#define BUILD_DECK_COUNT_BOX_FLAGS 0x1

#include "../build_deck_card_counts.c"
