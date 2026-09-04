#include "../types.h"

extern u16 D_8009B398[];
extern u8 D_8009B144[];
extern u8 D_8009B143[];
extern u8 D_8009B142[];
extern u32 D_8009B0F4[];
/* A separate linker name prevents GCC from retaining this address. */
extern u32 Base2_8009B0F4[];
extern u8 D_8009B318;

extern s32 func_8005C530(void);
extern void func_8005C5C4(void);

void func_800359B0(void)
{
    if (func_8005C530() == 0) {
        u8 value = D_8009B318;

        if ((value & 0x40) || (D_8009B398[0] & 0x800)) {
            D_8009B318 = value | 0x40;
            D_8009B144[0] = 1;
            D_8009B143[0] = 1;
            D_8009B142[0] = 1;
            func_8005C5C4();
        }
    } else {
        Base2_8009B0F4[0] = D_8009B0F4[0] & 0xFDFFFFFF;
        D_8009B318 &= 0x7F;
    }
}

void func_80035A58(void)
{
    D_8009B318 = 0;
}
