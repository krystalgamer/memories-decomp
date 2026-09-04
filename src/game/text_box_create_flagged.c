#include "../types.h"

extern void TextBox_SetRect(s32, s32, s32, s32, s32);
extern u8 *DuelEffect_InitEntry(s32, s32, s32);
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
