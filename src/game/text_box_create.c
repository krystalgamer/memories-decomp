#include "../types.h"

extern void TextBox_SetRect(s32, s32, s32, s32, s32);
extern void DuelEffect_InitEntry(s32, s32, s32);
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
