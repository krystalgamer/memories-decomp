#include "../types.h"
#include "../psyq/qsort.h"
#include "card_constants.h"
#include "duel_deck_card.h"
#include "file_transfer.h"

extern u16 gDuel_awCombinedDeckCardIds[];
extern u16 gDuel_awUniqueDeckCardIds[];
extern u8 D_8015C424[];
extern u8 D_8018C2D8[];

extern void Util_CopyWords(void *, void *, s32);
extern s32 Util_CompareS16(s16 *, s16 *);
extern void Duel_StepCardDataTransfer(FileTransferDescriptor *, s32);

void Duel_RequestCombinedDeckData(void)
{
    u8 *source = (u8 *)gDuel_awCombinedDeckCardIds;
    u8 *output;
    FileTransferDescriptor *result;
    s32 i;
    s32 previous;
    s32 first_id;
    u8 *table;
    s32 value;

    Util_CopyWords(
        source,
        source - DUEL_DECK_ID_BUFFER_STRIDE,
        COMBINED_DECK_SIZE * sizeof(u16)
    );
    qsort(source, COMBINED_DECK_SIZE, sizeof(u16), (int (*)())Util_CompareS16);

    output = source + DUEL_DECK_ID_BUFFER_STRIDE;
    previous = 0;
    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        value = *(u16 *)source;
        if (value != previous) {
            *(u16 *)output = value;
            previous = value;
            output += 2;
        }
        source += 2;
    }

    *(u16 *)output = DUEL_CARD_ID_LIST_END;
    table = D_8015C424;
    first_id = *(u16 *)(table + DUEL_UNIQUE_DECK_CARD_IDS_OFFSET);
    result = File_TryRequestAsyncTransfer(
        0, (u8 *)0, first_id - 1, previous - first_id + 1,
        Duel_StepCardDataTransfer, 0, 0
    );
    result->callback_data = gDuel_awUniqueDeckCardIds;
    result->position = (u32)table;
    D_8009B0F4 =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

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
