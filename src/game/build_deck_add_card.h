#ifndef MEMORIES_DECOMP_BUILD_DECK_ADD_CARD_H
#define MEMORIES_DECOMP_BUILD_DECK_ADD_CARD_H

#include "../types.h"

/* Fills the first free entry of the deck list at `base` + 0x2D58 with card
 * `card_id`: marks the entry used, writes the id, and copies the card's type,
 * attack and defense out of gDuel_adwCardStats[card_id - 1] with the same
 * shifts and CARD_STAT_SCALE the rest of the build-deck screen uses. Then it
 * re-sorts the list through func_80032C48 and refreshes the pane, and returns
 * -- so a full deck of DECK_SIZE entries is a silent no-op.
 *
 * `base` is s32 rather than a pointer because that is what the definition
 * takes: the screen record is reached by adding constants to it, never
 * dereferenced as a struct, and the arithmetic sits under a $a0 register pin.
 * Its one caller holds the same record as u8 * and casts. */
void BuildDeck_AddCard(s32 base, s32 card_id);

#endif
