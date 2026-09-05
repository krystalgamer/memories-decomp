#include "../types.h"

extern u32 D_8009B350;

extern s32 func_80036D3C(u8 *);
extern s32 Campaign_TestStoryFlag(s32);
extern void Library_UpdateCardUsedFlag(s32);

void func_80038D14(u8 *value)
{
    value[0x51] = 4;
    D_8009B350 = 1;
}

void func_80038D2C(u8 *object)
{
    s32 flag = func_80036D3C(object);

    flag &= 0xFFFF;
    if (flag & 0x4000) {
        Library_UpdateCardUsedFlag(flag & 0xBFFF);
        return;
    }

    {
        s32 target = func_80036D3C(object);

        target &= 0xFFFF;
        if (Campaign_TestStoryFlag(flag) != 0) {
            s32 *cursor = (s32 *)(object + *(s8 *)(object + 0x58) * 4);

            *cursor = (*cursor & 0xFFFF0000) | target;
        }
    }
}
