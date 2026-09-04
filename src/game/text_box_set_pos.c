#include "../types.h"

extern void func_80039140(u8 *);
extern void DuelEffect_UpdateObjectLayout(u8 *);

void TextBox_SetPos(u8 *record, s32 x, s32 y)
{
    u8 *object;
    object = *(u8 **)(record + 40);
    *(s16 *)(record + 60) = x;
    *(s16 *)(record + 64) = y;
    if (object != (u8 *)0) {
        *(s16 *)(object + 48) = x;
        *(s16 *)(object + 50) = y;
    }
    object = *(u8 **)(record + 44);
    if (object != (u8 *)0) {
        if (*(s16 *)(object + 30) == 4)
            func_80039140(record);
        else {
            *(s16 *)(object + 48) = x;
            *(s16 *)(object + 50) = y;
        }
    }
    object = *(u8 **)(record + 48);
    if (object != (u8 *)0) {
        if (*(s16 *)(object + 30) == 4)
            DuelEffect_UpdateObjectLayout(record);
        else {
            *(s16 *)(object + 48) = *(u16 *)(record + 62) + x - 16;
            *(s16 *)(object + 50) = *(u16 *)(record + 66) + y - 16;
        }
    }
}
