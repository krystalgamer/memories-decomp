#include "../../types.h"

/* SLPM-86398 build of src/game/build_deck.c. VERSION_JAPAN selects the
 * Japanese card-slot and input paths and the native label
 * build_deck_draw_sort_icons.h binds the sort-icon drawer to. The US source
 * is included unchanged. */

#define VERSION_JAPAN
#define CARD_LIST_GRAPHICS_REQUEST_FILE_ID 0x2184
#define BUILD_DECK_PANE_CANCEL_BUTTON PAD_BUTTON_CROSS
#define BUILD_DECK_PANE_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)

#include "../build_deck.c"
