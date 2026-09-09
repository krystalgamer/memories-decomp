#ifndef MEMORIES_DECOMP_DUEL_CARD_VIEWER_H
#define MEMORIES_DECOMP_DUEL_CARD_VIEWER_H

#include "../types.h"

/* The card id the viewer screen is currently showing.
 *
 * It is a publication point rather than a working variable: the pickers write
 * it and the viewer reads it, with nothing in between. build_deck_pane_input.c
 * stores the highlighted row's id at both of its pick sites, and
 * duel_update_card_pick_cursor.c does the same from the field cursor -- its
 * header comment describes a successful pick as being "published in
 * gDuel_wViewerCardID along with the event code 0x14 and state 2".
 *
 * func_800283F4.c is the consumer. It hands the id to func_80029164 to bring
 * the card's record in, and copies it into gDuel_wSelectedCardID for the
 * effect channels.
 *
 * Stored unsigned; readers cast to s16 before using it as a one-based index
 * into gDuel_adwCardStats. */
extern u16 gDuel_wViewerCardID;

#endif
