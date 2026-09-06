#include "../types.h"
#include "display_object_layout.h"

extern void func_80019B2C(void);

void func_80019BA0(u8 *arg0, u8 arg1, s16 arg2, s16 arg3)
{
    arg0[0x6C] = 1;
    arg0[0x21] = arg1;
    *(s16 *)(arg0 + 0x28) = arg2;
    *(s16 *)(arg0 + 0x2A) = arg3;
    *(void (**)(void))(arg0 + 0x24) = func_80019B2C;
    *(u16 *)(arg0 + 8) |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
}
