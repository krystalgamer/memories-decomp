#ifndef MEMORIES_DECOMP_DUEL_DECK_LOOKUP_H
#define MEMORIES_DECOMP_DUEL_DECK_LOOKUP_H

#include "../types.h"

/* Two lookups over the player's cards. Duel_FindPlayerDeckCard returns the
 * deck index of a card id, or -1 when it is not in the deck.
 * Library_CheckCardOwned answers the weaker question the Library screen asks:
 * a nonzero SaveDataState.card_quantities (trunk) entry returns 1 at once, and
 * otherwise the deck index decides -- so its -1 means "neither in the trunk
 * nor in the deck", the two places Library_MarkOwnedCards also walks. Its
 * caller supplies IDs 1..722. */
s32 Duel_FindPlayerDeckCard(s32 card_id);
s32 Library_CheckCardOwned(s32 card_id);

#endif
