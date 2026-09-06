#include "../../types.h"

extern u8 *D_801845B0[];
extern void MainMenu_UpdateValueWidgetTween(u8 *);

void MainMenu_StartValueWidgetTween(s32 index, s32 value)
{
    u8 *object = D_801845B0[index];

    object[0x6B] = index;
    *(s16 *)(object + 0x60) = 0xA;
    object[0x6C] = value;
    *(void **)(object + 0x24) = (void *)MainMenu_UpdateValueWidgetTween;
    *(s16 *)(object + 0x36) = *(u16 *)(object + 0x30);
    *(s16 *)(object + 0x38) = *(u16 *)(object + 0x32);
}
