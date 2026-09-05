#include "../types.h"

extern u16 gInput_wPad2Held, gInput_wPad2Pressed, gInput_wPad2Repeat;
extern u8 D_800EF6B0[];
extern u32 D_8009B390, D_8009B3B0, D_8009B3B4, D_8009B3A8;
void Input_ResetPads(void) {
    s32 i = 1;
    volatile u16 *c = &gInput_wPad2Held;
    volatile u16 *b = &gInput_wPad2Pressed;
    volatile u16 *a = &gInput_wPad2Repeat;
    u8 *base;
    do {
        *a = 0;
        *b = *a;
        *c = *b;
        a--; b--; c--; i--;
    } while (i >= 0);
    i = 0x1F;
    base = D_800EF6B0;
    for (; i >= 0; i--) {
        *(u8 *)((u32)i + (u32)base) = 0;
    }
    D_8009B390 = 0;
    D_8009B3B0 = 0;
    D_8009B3B4 = 0;
    D_8009B3A8 = 0;
}
