#ifndef MEMORIES_DECOMP_BUILD_DECK_DRAW_SORT_ICONS_H
#define MEMORIES_DECOMP_BUILD_DECK_DRAW_SORT_ICONS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* Draws the seven sort-menu icons of a Build Deck card list through one
 * scratchpad GsSPRITE, 18 pixels apart. `data` is the list kind's row of
 * D_80090DD8 byte pairs; the icon whose low nibble equals `selected` is
 * drawn at full grey, the others dimmed. `ot` is the ordering table it sorts
 * each icon into, which is what its one caller already holds. */
#ifdef VERSION_JAPAN
extern void BuildDeck_DrawSortIcons(
    GsSPRITE *sprite, GsOT *ordering_table, u8 *data, s32 selected)
    asm("func_80031784");
#else
void BuildDeck_DrawSortIcons(
    GsSPRITE *sprite, GsOT *ordering_table, u8 *data, s32 selected);
#endif

#ifdef VERSION_EUROPE
void CardList_DrawHeaderDecimalDigits(
    GsSPRITE *sprite, GsOT *ordering_table, u8 *digits, s32 count);
#endif

#endif
