#include "../types.h"

extern u8 gText_abColorSlots[], D_800EB15C[];
extern void TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);
void func_8003C4E0(s32 arg0) {
    u8 *t = gText_abColorSlots;
    t[0]=4; t[1]=4; t[2]=4; t[3]=4; t[4]=4;
    t[arg0]=0;
    if (arg0 != 0) t[3]=2; else t[4]=2;
    TextBox_Create(1,0xEF,0x18,0x38,0x120,0x100);
    func_80039A14(D_800EB15C);
}
