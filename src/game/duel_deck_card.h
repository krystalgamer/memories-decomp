#ifndef MEMORIES_DECOMP_DUEL_DECK_CARD_H
#define MEMORIES_DECOMP_DUEL_DECK_CARD_H

#include "../types.h"

typedef struct {
    s16 id;
    u8 index_02;
    u8 data_block_index;
    u8 flags_04;
    u8 unk_05;
} DuelDeckCardRecord;

extern DuelDeckCardRecord gDuel_aDeckCardRecords[];

/* The combined deck's card image data, indexed by the data_block_index field
 * above scaled by DUEL_CARD_DATA_BLOCK_SIZE. Duel_PopulateCombinedDeckData
 * fills it while it builds the records; Duel_SetupCardRecord then hands the
 * selected block straight to LoadImage. The record type and this buffer are
 * two halves of one thing, so they are declared together. */
extern u8 D_8018C2D8[];
u8 *Duel_SetupCardRecord(s32, s32);

#endif
