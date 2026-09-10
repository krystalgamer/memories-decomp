#ifndef MEMORIES_DECOMP_DUEL_DECK_CARD_DATA_H
#define MEMORIES_DECOMP_DUEL_DECK_CARD_DATA_H

#include "../types.h"

/* Builds the unique card id list for the combined deck and starts the fetch
 * for it.
 *
 * It copies the combined deck ids down into the sort buffer, sorts them with
 * qsort through Util_CompareS16, then walks the sorted run writing each id
 * that differs from the previous one into a second buffer, so the result is
 * the deck's ids deduplicated and in order, closed with a sentinel. It then
 * asks File_TryRequestAsyncTransfer for the block spanning the first id to
 * the last, with Duel_StepCardDataTransfer as the step callback, and records
 * the returned transfer's state with the primary-active bit set.
 *
 * The dedupe relies on the sort: it compares only against the previous
 * element, so it removes runs of equal ids rather than duplicates in
 * general. */
void Duel_RequestCombinedDeckData(void);

/* Fills gDuel_aDeckCardRecords once the transfer above has landed.
 *
 * For each of the combined deck's entries it takes the id and the flag byte
 * from their two tables, records the entry's own index twice, and then finds
 * that id in the unique id list to locate its data block, copying the block
 * to the running destination.
 *
 * That search is unbounded: it walks the unique id list until it finds a
 * match, with no end test, so it assumes every combined-deck id is present in
 * the list Duel_RequestCombinedDeckData built. That is a property of the pair
 * rather than of this function alone, which is why both belong to one unit.
 *
 * Both names are semantic and were already in place; this header only gives
 * them a home. */
void Duel_PopulateCombinedDeckData(void);

#endif
