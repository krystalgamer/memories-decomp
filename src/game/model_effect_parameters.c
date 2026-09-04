#include "../types.h"

extern u8 D_800F2B20[];
extern s16 D_800F2B22;

extern s32 func_8005F174(void);
extern s32 func_8005F18C(void);

void func_80059FAC(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        u8 *parameters = D_800F2B20;

        *(u16 *)(parameters + 0x12) = first;
        *(u16 *)(parameters + 0x1A) = second;
    }
}

void func_8005A010(s32 first, s32 second)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        u8 *parameters = D_800F2B20;

        *(u16 *)(parameters + 0x22) = first;
        *(u16 *)(parameters + 0x2A) = second;
    }
}

void func_8005A074(s32 value)
{
    s32 state = func_8005F174();

    if (state == 1 && func_8005F18C() == state) {
        return;
    }
    {
        u8 *parameters = D_800F2B20;

        *(u16 *)(parameters + 0xA) = (value < 0 ? -value : value) * 2;
        *(u16 *)(parameters + 8) = 0;
    }
}

void func_8005A0DC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        D_800F2B22 = value * 2;
    }
}

void func_8005A130(s32 value)
{
    s32 state = func_8005F174();
    u8 *parameters;

    if (state == 1) {
        if (func_8005F18C() == state) {
            return;
        }
    }
    parameters = D_800F2B20;
    *(s16 *)(parameters + 6) = value;
    *(s16 *)(parameters + 4) = value;
}
