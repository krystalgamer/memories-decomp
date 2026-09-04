#include "../types.h"

extern void func_80035CA8(s32);
extern void DuelEffect_ClearMatchingMarker(s32);
extern void func_8004036C(void *);

void TextBox_Destroy(u8 *record)
{
    func_80035CA8(record[0x57]);
    DuelEffect_ClearMatchingMarker(record[0x57]);
    *(u16 *)(record + 0x34) = 0;
    func_8004036C(*(void **)(record + 0x30));
    func_8004036C(*(void **)(record + 0x2C));
    func_8004036C(*(void **)(record + 0x28));
    *(void **)(record + 0x30) = 0;
    *(void **)(record + 0x2C) = 0;
    *(void **)(record + 0x28) = 0;
}
