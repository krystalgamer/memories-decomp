#include "../../types.h"
#include "name_entry_keyboard.h"
#include "name_entry_starter_deck.h"
#include "../../game/main_frame.h"
#include "../../game/graphics_frame.h"
#include "../../game/save_data.h"
#include "../../game/card_constants.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"
#include "../../game/campaign_flags.h"
#include "../../game/util_memory.h"

void NameEntry_BuildStarterDeck(void)
{
    u8 counts[CARD_COUNT];
    NameEntryStarterDeckPool **table;
    u16 *entry;
    u16 *p;
    s16 *out;
    s32 remaining;
    s32 acc;
    s32 threshold;
    s32 i;

    for (i = CARD_COUNT - 1; i >= 0; i--) {
        counts[i] = 0;
    }
    out = (s16 *)gDuel_awPlayerDeck;
    table = gNameEntry_apStarterDeckPools;
    entry = (u16 *)*table;
    while (entry != 0) {
        remaining = *entry;
        entry++;
        do {
            threshold = (rand() & 0x7FF) + 1;
            acc = 0;
            i = 0;
            p = entry;
            do {
                rand();
                acc += *p;
                if (acc >= threshold) {
                    if (counts[i] >= 3) {
                        remaining++;
                    } else {
                        counts[i] = counts[i] + 1;
                        *out = i + 1;
                        Library_UpdateCardUsedFlag(i + 289);
                        out++;
                    }
                    break;
                }
                i++;
                p++;
            } while (i < STARTER_DECK_WEIGHT_SCAN_COUNT);
            remaining--;
        } while (remaining != 0);
        table++;
        entry = (u16 *)*table;
    }
}

void NameEntry_Main(void)
{
    SaveDataState *state;
    u8 *entry;
    s32 checksum;
    s32 value;
    s32 i;

    Util_FillMemory(D_801D0000, 0, 0x3000);
    printf("SaveLoadBuf add = 0x%x size = 0x%x\n", D_801D0000, 0x3000);
    NameEntry_Init();
    do {
        func_80012D4C();
        rand();
    } while (NameEntry_PollCompletion() == 0);
    NameEntry_BuildStarterDeck();
    state = (SaveDataState *)gDuel_awPlayerDeck;
    checksum = 0;
    entry = state->player_name_sjis;
    for (i = SAVE_DATA_PLAYER_NAME_SIZE - 1; i >= 0; i--) {
        checksum ^= *entry;
        entry++;
    }
    value = D_8009B09C << 8;
    while ((state->duelist_code = value | checksum) == 0) {
        value = rand() << 8;
    }
}
