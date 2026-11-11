#include "../../types.h"

/* SLES-03947 build of src/game/card_list_deck_box.c, with its European bodies. */

#define VERSION_EUROPE

#define CARD_LIST_ROW_ENTRY_COUNT 8
#define CARD_LIST_DIGIT_TPAGE 0xA
#define CARD_LIST_DIGIT_CX 0x280
#define CARD_LIST_DIGIT_CY 0xC0
#define CARD_LIST_DIGIT_V 0x40
#define CARD_LIST_ICON_CX(page) ((page) + 0x280)

#define CARD_LIST_FOLLOWING_FLAG 0x80

/* CardList_CreateDeckBoxStatRows reads the viewport through these literal
 * addresses. The four language builds define their own first. */
#ifndef gGraphics_sViewportY_abs
#define gGraphics_sViewportY_abs (*(s16 *)0x8009C0A2)
#define gGraphics_sViewportX_abs (*(s16 *)0x8009C0A0)
#endif
#include "../card_list_deck_box.c"
