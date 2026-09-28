#include "../../types.h"

/* SLES-03947 build of src/game/card_list_render_deck_box_stats.c, with its European bodies. */

#define VERSION_EUROPE

#define CARD_LIST_ROW_ENTRY_COUNT 8
#define CARD_LIST_DIGIT_TPAGE 0xA
#define CARD_LIST_DIGIT_CX 0x280
#define CARD_LIST_DIGIT_CY 0xC0
#define CARD_LIST_DIGIT_V 0x40
#define CARD_LIST_ICON_CX(page) ((page) + 0x280)

#include "../card_list_render_deck_box_stats.c"
