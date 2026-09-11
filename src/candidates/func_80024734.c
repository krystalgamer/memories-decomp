/* Reclassified from matching_c (#3859). This was part of
 * src/game/duel_deck_card_data.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 60 of 60 instructions with 8 differing, opcode distance
 * 0. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/duel_deck_card_data.h"
#include "../psyq/qsort.h"
#include "../game/card_constants.h"
#include "../game/duel_card_data_transfer.h"
#include "../game/duel_deck_card.h"
#include "../game/duel_card_staging.h"
#include "../game/file_transfer.h"
#include "../game/util_memory.h"
#include "../game/util_compare_s16.h"

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
