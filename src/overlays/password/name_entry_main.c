#include "../../types.h"
#include "../../game/save_data.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

extern u8 D_801D0000[];
extern u8 D_801D0200[];
extern s32 D_8009B09C;
extern void func_80035748(void *, s32, s32);
extern void NameEntry_Init(void);
extern void func_80012D4C(void);
extern s32 NameEntry_PollCompletion(void);
extern void NameEntry_BuildStarterDeck(void);

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
    state = D_801D0200;
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
