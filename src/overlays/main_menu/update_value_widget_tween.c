#include "../../types.h"

extern u8 *D_801845B8;
extern u8 D_801845BC[];
extern u16 D_801845C0[];

void MainMenu_UpdateValueWidgetTween(u8 *obj)
{
    u8 *widget;
    s32 targetX;
    s32 targetY;
    s32 valueA;
    s32 valueB;
    s16 remaining;

    widget = D_801845B8;
    targetX = *(s16 *)(widget + 0x30);
    targetY = *(s16 *)(widget + 0x32);
    valueA = D_801845C0[1];
    valueB = D_801845C0[7];
    if (obj[0x6C] == 2) {
        if (obj[0x6B] == 0) {
            targetX = (valueA * 128) / 8000 + 176;
            targetY = 111;
        } else {
            targetX = (valueB * 128) / 8000 + 176;
            targetY = 139;
        }
    }
    remaining = *(u16 *)(obj + 0x60) - 1;
    *(s16 *)(obj + 0x60) = remaining;
    if (remaining <= 0) {
        D_801845BC[obj[0x6B]] = obj[0x6C];
        *(s32 *)(obj + 0x24) = 0;
    }
    *(s16 *)(obj + 0x30) =
        (*(s16 *)(obj + 0x36) * *(s16 *)(obj + 0x60)) / 10 +
        (targetX * (10 - *(s16 *)(obj + 0x60))) / 10;
    *(s16 *)(obj + 0x32) =
        (*(s16 *)(obj + 0x38) * *(s16 *)(obj + 0x60)) / 10 +
        (targetY * (10 - *(s16 *)(obj + 0x60))) / 10;
}
