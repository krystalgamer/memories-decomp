#include "../../types.h"
#include "../../psyq/rand.h"

extern u8 D_801D0000[];
extern u8 D_801D0200[];
extern s32 D_8009B09C;
extern void func_80035748(void *, s32, s32);
extern void func_8008E870(const char *, void *, s32);
extern void func_801683EC(void);
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
    func_8008E870("SaveLoadBuf add = 0x%x size = 0x%x\n", D_801D0000, 0x3000);
    func_801683EC();
    do {
        func_80012D4C();
        rand();
    } while (NameEntry_PollCompletion() == 0);
    NameEntry_BuildStarterDeck();
    state = D_801D0200;
    checksum = 0;
    entry = state + 0x40C;
    for (i = 0xB; i >= 0; i--) {
        checksum ^= *entry;
        entry++;
    }
    value = D_8009B09C << 8;
    while ((*(s32 *)(state + 0x334) = value | checksum) == 0) {
        value = rand() << 8;
    }
}
