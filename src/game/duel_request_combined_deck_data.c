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
    *(u32 *)0x8009B0F4 =
        result->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}
