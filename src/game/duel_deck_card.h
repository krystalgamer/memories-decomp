#ifndef MEMORIES_DECOMP_DUEL_DECK_CARD_H
#define MEMORIES_DECOMP_DUEL_DECK_CARD_H

#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

typedef struct {
    u8 pixels[0x500];
    u16 clut[64];
} DuelCardThumbnailBlock;

typedef char DuelCardThumbnailBlock_size_must_be_0x580[
    sizeof(DuelCardThumbnailBlock) == DUEL_CARD_DATA_BLOCK_SIZE ? 1 : -1
];
typedef char DuelCardThumbnailBlock_clut_offset_must_be_0x500[
    (u32)&(((DuelCardThumbnailBlock *)0)->clut) == 0x500 ? 1 : -1
];

typedef struct {
    s16 id;
    /* Source slot in the combined deck. Draw and placement paths consume it
       with signed byte loads before passing it to Duel_SetupCardRecord. */
    s8 deck_index;
    u8 data_block_index;
    /* Slot before shuffle, kept paired with the ID; the marker stores this + 1. */
    u8 pre_shuffle_index;
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
    DUEL_DECK_CARD_OFFSET(deck_index) == 2 ? 1 : -1
];
typedef char DuelDeckCardRecord_data_block_offset_must_be_3[
    DUEL_DECK_CARD_OFFSET(data_block_index) == 3 ? 1 : -1
];
typedef char DuelDeckCardRecord_pre_shuffle_index_offset_must_be_4[
    DUEL_DECK_CARD_OFFSET(pre_shuffle_index) == 4 ? 1 : -1
];
typedef char DuelDeckCardRecord_unknown_offset_must_be_5[
    DUEL_DECK_CARD_OFFSET(unk_05) == 5 ? 1 : -1
];
#undef DUEL_DECK_CARD_OFFSET

extern DuelDeckCardRecord gDuel_aDeckCardRecords[];

/* Combined-deck thumbnail blocks, indexed by data_block_index. Population
 * copies each complete block; preview readback can replace it as 8x88 GPU
 * words. Setup uploads its 40x32 8-bpp pixels and 64-entry CLUT separately. */
extern DuelCardThumbnailBlock gDuel_aCardThumbnailBlocks[];
/* Returns the initialized live record; the slot/deck words keep their
 * existing full-width tagged-index conventions. */
DuelCardRecord *Duel_SetupCardRecord(s32, s32);

#endif
