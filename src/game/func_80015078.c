#include "../types.h"

extern volatile s32 D_8009B0F4;
extern u8 D_800E9E18[];
extern void func_80015010(void);
extern u8 *File_InitTransferDescriptor(u8 *, s32, u8 *, s32, s32, void *, s32, s32);

u8 *func_80015078(s32 arg0, u8 *arg1, s32 arg2, s32 arg3, void *arg4,
                  s32 arg5, s32 arg6)
{
    u8 *state;

    D_8009B0F4 &= ~0x20;
    if ((D_8009B0F4 & 0x10) && (D_8009B0F4 & 0x80000)) {
        func_80015010();
    }

    state = D_800E9E18;
    File_InitTransferDescriptor(state, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    D_8009B0F4 |= 0x20;
    return state;
}
