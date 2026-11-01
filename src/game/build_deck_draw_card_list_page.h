#ifndef MEMORIES_DECOMP_BUILD_DECK_DRAW_CARD_LIST_PAGE_H
#define MEMORIES_DECOMP_BUILD_DECK_DRAW_CARD_LIST_PAGE_H

#include "../types.h"
#include "display_object.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws one page of a Build Deck card list: the sort-menu icons above it and
 * eight rows (nine in Europe) of card id and ATK/DEF. The object's field_67
 * picks the list; list 0 also marks ranked cards and shows chest and deck counts,
 * the other list numbers its rows instead. No C code calls it:
 * BuildDeck_InitScreen stores its address in a display object's field_4C,
 * the (DisplayObject *, GsOT *) draw callback slot. */
void BuildDeck_DrawCardListPage(DisplayObject *obj, GsOT *ot);

#include "build_deck_draw_card_list_page_data.h"

#endif
