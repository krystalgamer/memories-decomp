#include "../types.h"
#include "../psyq/qsort.h"
#include "card_constants.h"
#include "file_transfer.h"

typedef struct {
    u16 id;
    u8 b2;
    u8 b3;
    u8 b4;
    u8 b5;
} DeckCardRecord;

extern u8 D_8017808C[];
extern u8 D_80178130[];
extern u8 D_8015C424[];
extern u8 D_8018C2D8[];
extern DeckCardRecord gDuel_aDeckCardRecords[];
extern u32 D_8009B0F4;

extern void Util_CopyWords(void *, void *, s32);
extern s32 Util_CompareS16(s16 *, s16 *);
extern void Duel_StepCardDataTransfer(FileTransferDescriptor *, s32);

void Duel_RequestCombinedDeckData(void)
{
    u8 *source = D_8017808C;
    u8 *output;
    FileTransferDescriptor *result;
    s32 i;
    s32 previous;
    s32 count;
    u8 *table;
    s32 value;

    Util_CopyWords(
        source,
        source - DUEL_DECK_ID_BUFFER_SIZE,
        COMBINED_DECK_SIZE * sizeof(u16)
    );
    qsort(source, COMBINED_DECK_SIZE, sizeof(u16), (int (*)())Util_CompareS16);

    output = source + DUEL_DECK_ID_BUFFER_SIZE;
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

    *(u16 *)output = 0xFFFF;
    table = D_8015C424;
    count = *(u16 *)(table + 0x1BD0C);
    result = File_TryRequestAsyncTransfer(
        0, (u8 *)0, count - 1, previous - count + 1,
        Duel_StepCardDataTransfer, 0, 0
    );
    result->callback_data = D_80178130;
    result->position = (u32)table;
    D_8009B0F4 = result->status_flags | 0x10;
}

void Duel_PopulateCombinedDeckData(void)
{
    u8 *dst = D_8018C2D8;
    DeckCardRecord *rec = gDuel_aDeckCardRecords;
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
        rec->b4 = r[DUEL_COMBINED_DECK_CARD_FLAGS_OFFSET];
        rec->b2 = i;
        rec->b3 = i;

        src = D_8015C424;
        p = (u16 *)D_80178130;
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
