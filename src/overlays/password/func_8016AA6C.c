#include "../../types.h"

extern u8 D_801D0000[];
extern u8 D_801D0200[];
extern u8 D_80168090[];
extern s32 D_8009B09C;
extern void func_80035748(void *, s32, s32);
extern void func_8008E870(void *, void *, s32);
extern void func_801683EC(void);
extern void func_80012D4C(void);
extern s32 func_8008E590(void);
extern s32 func_80169C08(void);
extern void func_8016A930(void);

void func_8016AA6C(void)
{
    u8 *state;
    u8 *entry;
    s32 checksum;
    s32 value;
    s32 i;

    func_80035748(D_801D0000, 0, 0x3000);
    func_8008E870(D_80168090, D_801D0000, 0x3000);
    func_801683EC();
    do {
        func_80012D4C();
        func_8008E590();
    } while (func_80169C08() == 0);
    func_8016A930();
    state = D_801D0200;
    checksum = 0;
    entry = state + 0x40C;
    for (i = 0xB; i >= 0; i--) {
        checksum ^= *entry;
        entry++;
    }
    value = D_8009B09C << 8;
    while ((*(s32 *)(state + 0x334) = value | checksum) == 0) {
        value = func_8008E590() << 8;
    }
}
