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

typedef char DuelDeckCardRecord_size_must_be_6[
    sizeof(DuelDeckCardRecord) == 6 ? 1 : -1
];
#define DUEL_DECK_CARD_OFFSET(member) ((u32)&(((DuelDeckCardRecord *)0)->member))
typedef char DuelDeckCardRecord_id_offset_must_be_0[
    DUEL_DECK_CARD_OFFSET(id) == 0 ? 1 : -1
];
typedef char DuelDeckCardRecord_index_offset_must_be_2[
    DUEL_DECK_CARD_OFFSET(index_02) == 2 ? 1 : -1
];
typedef char DuelDeckCardRecord_data_block_offset_must_be_3[
    DUEL_DECK_CARD_OFFSET(data_block_index) == 3 ? 1 : -1
];
typedef char DuelDeckCardRecord_flags_offset_must_be_4[
    DUEL_DECK_CARD_OFFSET(flags_04) == 4 ? 1 : -1
];
typedef char DuelDeckCardRecord_unknown_offset_must_be_5[
    DUEL_DECK_CARD_OFFSET(unk_05) == 5 ? 1 : -1
];
#undef DUEL_DECK_CARD_OFFSET

extern DuelDeckCardRecord gDuel_aDeckCardRecords[];

/* The combined deck's card image data, indexed by the data_block_index field
 * above scaled by DUEL_CARD_DATA_BLOCK_SIZE. Duel_PopulateCombinedDeckData
 * fills it while it builds the records; Duel_SetupCardRecord then hands the
 * selected block straight to LoadImage. The record type and this buffer are
 * two halves of one thing, so they are declared together. */
extern u8 D_8018C2D8[];
u8 *Duel_SetupCardRecord(s32, s32);

#endif
