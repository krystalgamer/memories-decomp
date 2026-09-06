#include "../../types.h"

extern s32 D_801D07E0;
extern s32 D_801D5608;
extern u8 D_800EB0F8[];
extern void func_8003B6AC(int, int);
extern void func_80035BE4(int, int, int, int, int, int);
extern void func_80039A14(void *);

void Password_RefreshStarchipDisplay(void)
{
    D_801D5608 = D_801D07E0;
    func_8003B6AC(3, 1);
    func_80035BE4(3, 0xE1, 0x98, 0x28, 0xA0, 0x20);
    D_800EB0F8[0x186] = 0x10;
    D_800EB0F8[0x187] = 0x10;
    func_80039A14(&D_800EB0F8[0x12C]);
}
