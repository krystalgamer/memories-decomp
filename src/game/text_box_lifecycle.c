#include "../types.h"

extern void func_80035CA8(s32);
extern void DuelEffect_ClearMatchingMarker(s32);
extern void func_8004036C(void *);
extern void TextBox_SetRect(s32, s32, s32, s32, s32);
extern u8 *DuelEffect_InitEntry(s32, s32, s32);

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

void TextBox_Create(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height
)
{
    TextBox_SetRect(index, x, y, width, height);
    DuelEffect_InitEntry(index, string_id, 0);
}

u8 *TextBox_CreateFlagged(
    s32 index,
    s32 string_id,
    s32 x,
    s32 y,
    s32 width,
    s32 height,
    s32 flags
)
{
    u8 *result;

    TextBox_SetRect(index, x, y, width, height);
    result = DuelEffect_InitEntry(index, string_id, 0);
    *(u16 *)(result + 0x34) |= flags;
    return result;
}
