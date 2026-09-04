#include "../types.h"

extern s32 func_8005F174(void);
extern s32 func_8005F18C(void);
extern void func_80052D2C(s32, s32, s32, s32);

void func_80059EBC(s32 value)
{
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(value, 0, 0, 0);
    }
}

void func_80059F18(s32 first, s32 second, s32 third, s32 fourth)
{
    s32 adjusted_second = second < 0 ? 0 : second + 1;
    s32 adjusted_third = third < 0 ? 0 : third + 1;
    s32 state = func_8005F174();

    if (state != 1 || func_8005F18C() != state) {
        func_80052D2C(first, adjusted_second, adjusted_third, fourth);
    }
}
