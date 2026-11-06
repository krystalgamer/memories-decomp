#include "../../types.h"

/* SLES-03947 build of src/game/build_deck_input.c. */

#define VERSION_EUROPE
#define CARD_LIST_VISIBLE_ROWS 9
#define CARD_LIST_LAST_CURSOR 8
#define CARD_LIST_SCROLL_SCALE 175
#define CARD_LIST_SCROLL_Y_BASE 0x2A
#define CARD_LIST_CURSOR_Y_BASE 0x2B
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define BUILD_DECK_DECK_PANE_X 0x235
#define BUILD_DECK_DECK_PANE_Y 0x14
#define BUILD_DECK_DECK_PANE_ARG4 0xC
#define BUILD_DECK_CHEST_PANE_Y 0x14
#define BUILD_DECK_CHEST_PANE_ARG4 0xD
#define BUILD_DECK_WIDE_BOX_Y 0x84
#define BUILD_DECK_WIDE_BOX_FLAGS 0x1050
#define BUILD_DECK_NARROW_BOX_Y 0x6C
#define BUILD_DECK_NARROW_BOX_FLAGS 0x40
#define BUILD_DECK_CONFIRM_FADE_LEVEL 0xC0
#define BUILD_DECK_CONFIRM_COMPLETION_MASK 0x2010
#define BUILD_DECK_DIVISOR_LADDER_ONLY

#include "../build_deck_input.c"
