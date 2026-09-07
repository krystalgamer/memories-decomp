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

#endif
