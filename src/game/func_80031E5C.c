#include "../types.h"
#include "display_object_layout.h"

extern u32 D_801D5608[];
extern u8 *TextBox_CreateFlagged(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);
void func_80031E5C(u8 *arg0) {
    u8 *p;
    D_801D5608[0] = *(u32 *)(arg0 + 0x5A9C);
    D_801D5608[1] = *(u32 *)(arg0 + 0x5AA0);
    p = TextBox_CreateFlagged(3, 0xE, 0x16, 0x17, 0x280, 0x10, 0x100);
    func_80039A14(p);
    *(u16 *)(*(u8 **)(p + 0x28) + 8) &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}
