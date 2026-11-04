#ifndef MEMORIES_DECOMP_CARD_PRESENTATION_H
#define MEMORIES_DECOMP_CARD_PRESENTATION_H

#include "duel_card.h"
#include "duel_card_turn_animations.h"

/* The same table, entered five records in: 0x801A7B64 is 0x8C past
 * gDuel_aCardRecords and 0x8C is 5 * DUEL_CARD_RECORD_SIZE, so this is
 * &gDuel_aCardRecords[5]. Duel_CollectMatchingFieldCardObjects reaches the
 * field rows through this name for its two-side traversal and the original
 * relocation. Duel_CollectFieldRowCardObjects selects its row through
 * gDuel_aCardRecords instead. */
extern DuelCardRecord gDuel_aFieldCardRecords[];

#endif
