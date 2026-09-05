#include "../types.h"

extern u8 D_800EF6B0[];
/* Retail rematerializes this address inside the repeat loop. */
extern u8 D_8009B0D8 __attribute__((section(".data")));
extern u32 D_8009B0C8[];
extern u32 D_8009B390;
/* Preserve the low-half/high-half publication order at the function tail. */
extern volatile u16 D_8009B394;
extern volatile u16 D_8009B396;
extern volatile u16 D_8009B398;
extern volatile u16 D_8009B39A;
extern u8 D_8009B39C;
extern u8 D_8009B3A2;
extern volatile u16 D_8009B3A4;
extern volatile u16 D_8009B3A6;
extern u32 D_8009B3A8;
extern u32 D_8009B3B0;
extern u32 D_8009B3B4;

void func_8003CCD8(void)
{
    s32 i;
    u32 repeat;
    u32 current;
    u32 newly_pressed;
    u32 held;
    u32 new_bits;
    u8 value;

    repeat = 0;
    current = D_8009B3A8;
    D_8009B3A8 = 0;
    held = current;
    newly_pressed = (D_8009B390 ^ current) & current;
    new_bits = newly_pressed;
    D_8009B390 = current;

    for (i = 31; i >= 0; i--) {
        repeat <<= 1;
        if (held & 0x80000000) {
            if (new_bits & 0x80000000) {
                repeat |= 1;
            }
            value = D_800EF6B0[i] + D_8009B0D8;
            D_800EF6B0[i] = value;
            if (value >= D_8009B39C) {
                D_800EF6B0[i] = D_8009B3A2;
                repeat |= 1;
            }
        } else {
            D_800EF6B0[i] = 0;
        }
        held <<= 1;
        new_bits <<= 1;
    }

    if (D_8009B0C8[0] != 0) {
        D_8009B3B0 |= repeat;
        D_8009B3B4 |= newly_pressed;
    } else {
        newly_pressed |= D_8009B3B4;
        repeat |= D_8009B3B0;
        D_8009B3B0 = 0;
        D_8009B3B4 = 0;
    }
    D_8009B3A4 = current;
    D_8009B3A6 = current >> 16;
    D_8009B398 = newly_pressed;
    D_8009B39A = newly_pressed >> 16;
    D_8009B394 = repeat;
    D_8009B396 = repeat >> 16;
}
