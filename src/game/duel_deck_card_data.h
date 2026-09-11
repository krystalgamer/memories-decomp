#ifndef MEMORIES_DECOMP_DUEL_DECK_CARD_DATA_H
#define MEMORIES_DECOMP_DUEL_DECK_CARD_DATA_H

#include "../types.h"

void Duel_RequestCombinedDeckData(void);

/* Fills gDuel_aDeckCardRecords once the transfer Duel_RequestCombinedDeckData
 * starts has landed.
 *
 * For each of the combined deck's entries it takes the id and the flag byte
 * from their two tables, records the entry's own index twice, and then finds
 * that id in the unique id list to locate its data block, copying the block
 * to the running destination.
 *
 * That search is unbounded: it walks the unique id list until it finds a
 * match, with no end test, so it assumes every combined-deck id is present in
 * the list Duel_RequestCombinedDeckData built. That is a property of the pair
 * rather than of this function alone. */
void Duel_PopulateCombinedDeckData(void);

#endif
