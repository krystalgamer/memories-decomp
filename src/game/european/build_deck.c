#include "../../types.h"

/* SLES-03947 to SLES-03951 build of src/game/build_deck.c. The US source is
 * included as is; the values below differ in the European release and the US
 * source names each with an #ifndef default (jp_promote.REGIONAL). */

#define VERSION_EUROPE

/* Card transfer icon and card stat display. */
#define DUEL_CARD_TYPE_ICON_FIELD_5D 0xC0
#define DUEL_CARD_TYPE_ICON_CX 0x2E0
#define DUEL_CARD_STAT_DIGIT_V 0x40

/* Card slots and the deck count box. */
#define CARD_LIST_SLOT_BOX_Y 0x2C
#define CARD_LIST_SLOT_BOX_HEIGHT 0xE0
#define CARD_LIST_SLOT_FOLLOWING_FLAG 0x80
#define BUILD_DECK_COUNT_BOX_Y 0x19
#define BUILD_DECK_COUNT_BOX_FLAGS 0x1

/* The European card-list graphics request: 0x51 sectors from 0x2997. */
#define CARD_LIST_GRAPHICS_REQUEST_FILE_ID 0x2997
#define CARD_LIST_GRAPHICS_REQUEST_SECTOR_COUNT 0x51

/* Screen setup. */
#define BUILD_DECK_CURSOR_Y 0x2A
#define BUILD_DECK_LIST_Y 0x2B

/* List, pane and confirm input. */
#define CARD_LIST_VISIBLE_ROWS 9
#define CARD_LIST_LAST_CURSOR 8
#define CARD_LIST_SCROLL_SCALE 175
#define CARD_LIST_SCROLL_Y_BASE 0x2A
#define CARD_LIST_CURSOR_Y_BASE 0x2B
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

#include "../build_deck.c"
