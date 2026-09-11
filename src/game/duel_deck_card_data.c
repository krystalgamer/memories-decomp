#include "../types.h"
#include "duel_deck_card_data.h"
#include "../psyq/qsort.h"
#include "card_constants.h"
#include "duel_card_data_transfer.h"
#include "duel_deck_card.h"
#include "duel_card_staging.h"
#include "file_transfer.h"
#include "util_memory.h"
#include "util_compare_s16.h"

void Duel_PopulateCombinedDeckData(void)
{
    u8 *dst = D_8018C2D8;
    DuelDeckCardRecord *rec = gDuel_aDeckCardRecords;
    u8 *q;
    u8 *src;
    u8 *r;
    u16 *p;
    s32 i;
    s32 id;
    s32 w;
    u16 v;

    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        q = D_8015C424 + i * 2;
        v = *(u16 *)(q + DUEL_COMBINED_DECK_CARD_IDS_OFFSET);
        rec->id = v;
        id = (s16)v;
        r = D_8015C424 + i;
        rec->flags_04 = r[DUEL_COMBINED_DECK_CARD_FLAGS_OFFSET];
        rec->index_02 = i;
        rec->data_block_index = i;

        src = D_8015C424;
        p = gDuel_awUniqueDeckCardIds;
    search:
        w = *p;
        p++;
        if (w != id) {
            src += DUEL_CARD_DATA_BLOCK_SIZE;
            goto search;
        }
        Util_CopyWords(dst, src, DUEL_CARD_DATA_BLOCK_SIZE);
        dst += DUEL_CARD_DATA_BLOCK_SIZE;
        rec++;
    }
}
