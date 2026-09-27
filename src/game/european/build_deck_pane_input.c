#include "../../types.h"

/* SLES-03947 build of src/game/build_deck_pane_input.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define BUILD_DECK_DECK_PANE_X 0x235
#define BUILD_DECK_DECK_PANE_Y 0x14
#define BUILD_DECK_DECK_PANE_ARG4 0xC
#define BUILD_DECK_CHEST_PANE_Y 0x14
#define BUILD_DECK_CHEST_PANE_ARG4 0xD

#include "../build_deck_pane_input.c"
