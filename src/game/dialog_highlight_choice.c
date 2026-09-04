#include "../types.h"

extern u8 D_8009B336;
extern s8 D_8009B34D;
extern void DuelEffect_UpdateObjectLayout(void *);

void Dialog_HighlightChoice(u8 *record)
{
    s32 value;
    u8 *sub = *(u8 **)(record + 0x30);

    if (((D_8009B336 >> D_8009B34D) & 1) == 0) value = 0xC0;
    else value = 0xC0C0;
    *(s32 *)(sub + 0x0C) = value;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x3C) = value;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x44) = value;
    value = value / 4;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x2C) = value;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x34) = value;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x4C) = value;
    *(s32 *)(*(u8 **)(record + 0x30) + 0x54) = value;
    DuelEffect_UpdateObjectLayout(record);
}
