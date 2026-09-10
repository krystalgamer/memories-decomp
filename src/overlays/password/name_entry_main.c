#include "../../types.h"
#include "name_entry_keyboard.h"
#include "../../game/main_frame.h"
#include "../../game/graphics_frame.h"
#include "../../game/save_data.h"
#include "../../game/card_constants.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

extern u16 *D_8016D3DC[];
extern u8 D_801D0000[];
extern void func_8002CCE4(s32);
extern void func_80035748(void *, s32, s32);

void NameEntry_BuildStarterDeck(void)
{
    u8 counts[CARD_COUNT];
    u16 **table;
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
    table = D_8016D3DC;
    entry = *table;
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
                        func_8002CCE4(i + 289);
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
        entry = *table;
    }
}

void NameEntry_Main(void)
{
    u8 *state;
    u8 *entry;
    s32 checksum;
    s32 value;
    s32 i;

    func_80035748(D_801D0000, 0, 0x3000);
    printf("SaveLoadBuf add = 0x%x size = 0x%x\n", D_801D0000, 0x3000);
    NameEntry_Init();
    do {
        func_80012D4C();
        rand();
    } while (NameEntry_PollCompletion() == 0);
    NameEntry_BuildStarterDeck();
    state = (u8 *)gDuel_awPlayerDeck;
    checksum = 0;
    entry = state + SAVE_DATA_PLAYER_NAME_OFFSET;
    for (i = SAVE_DATA_PLAYER_NAME_SIZE - 1; i >= 0; i--) {
        checksum ^= *entry;
        entry++;
    }
    value = D_8009B09C << 8;
    while ((*(s32 *)(state + SAVE_DATA_DUELIST_CODE_OFFSET) =
                value | checksum) == 0) {
        value = rand() << 8;
    }
}
