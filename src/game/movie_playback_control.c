#include "../types.h"
#include "input.h"

extern u16 gInput_wPad1Pressed[];
extern u8 D_8009B144[];
extern u8 D_8009B143[];
extern u8 D_8009B142[];
extern u32 D_8009B0F4 __attribute__((section(".data")));
/* Preserve the array-form load without changing the scalar update above. */
extern u32 D_8009B0F4_array[] asm("D_8009B0F4");
/* A separate linker name prevents GCC from retaining this address. */
extern u32 Base2_8009B0F4[];
extern u8 D_8009B318;

extern void func_800137E4(void);
extern void func_80044F58(s32);
extern void func_8005C388(s32, s32, s32, s32, s32);
extern s32 func_8005C530(void);
extern void func_8005C5C4(void);

void func_8003594C(s32 arg0)
{
    func_800137E4();
    func_80044F58(0xFF);
    D_8009B318 = 0x80;
    D_8009B0F4 |= 0x2000000;
    func_8005C388(arg0, 1, -1, 1, 0);
}

void func_800359B0(void)
{
    if (func_8005C530() == 0) {
        u8 value = D_8009B318;

        if ((value & 0x40) ||
            (gInput_wPad1Pressed[0] & PAD_BUTTON_START)) {
            D_8009B318 = value | 0x40;
            D_8009B144[0] = 1;
            D_8009B143[0] = 1;
            D_8009B142[0] = 1;
            func_8005C5C4();
        }
    } else {
        Base2_8009B0F4[0] = D_8009B0F4_array[0] & 0xFDFFFFFF;
        D_8009B318 &= 0x7F;
    }
}

void func_80035A58(void)
{
    D_8009B318 = 0;
}
